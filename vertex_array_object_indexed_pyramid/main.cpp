/* matrices */

// some basic headers
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM include files
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtc/matrix_transform.hpp>  

// shader init
#include "init_shaders.h"
using namespace std;

// window
int window_width  = 1024;
int window_heigth = 768;

// user interaction
GLfloat wsc = 5.0f;

// shader program
GLuint shaderProgram;

// martrices
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;		// Store the view matrix
glm::mat4 modelMatrix;		// Store the model matrix

// attributes
GLuint attrib_Position;
//GLuint attrib_Color;

// Vertex Array Object
GLuint VAO;


static const char* pVS = "                                                          \n\
#version 400                                                                        \n\
			                                                                        \n\
layout (location = 0) in vec3 Position;		                                        \n\
layout (location = 1) in vec4 Color;   										        \n\
out vec4 vColor;																	\n\
uniform mat4 projectionMatrix;														\n\
uniform mat4 viewMatrix;															\n\
uniform mat4 modelMatrix;															\n\
																					\n\
void main()                                                                         \n\
{                                                                                   \n\
	vColor = vec4(Position.x, Position.y, Position.z, 1.0);                                              \n\
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(Position, 1.0);\n\
}";

static const char* pFS = "                                                          \n\
#version 400                                                                        \n\
in vec4 vColor;                                                                     \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
	                                                                                \n\
    FragColor = vColor;																\n\
}";


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.7, 0.7, 0.7, 1.0);

	glUseProgram(shaderProgram);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create our perspective projection matrix
	projectionMatrix = glm::perspective(100.0f, (float)window_width / (float)window_heigth, 0.1f, 100.0f);

	viewMatrix = glm::lookAt(
		glm::vec3(0, wsc, 3),		
		glm::vec3(0, 0, 0),		// look at this point
		glm::vec3(0, 1, 0)		// some kind of turn the camera
		);
	static int a = 0;
	//modelMatrix = glm::rotate(modelMatrix, 1.0f, glm::vec3(0, 1, 0));

	int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix"); 
	int viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix"); 

	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); 

	// draw triangle

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// onyl one Vertex Buffer Object can be bound simultaneously
	glBindVertexArray(VAO);
	
	// Enable the attribute after vertex array object is bound
	// and directly before the draw call
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, 6*3, GL_UNSIGNED_INT, 0);
	// Disable the attribute after vertex array object is bound
	// and directly after the draw call
	glDisableVertexAttribArray(0);

	glBindVertexArray(0);

    glutSwapBuffers();
}

void init_resources()
{
	// Create pyramid

	// Buffer Object array (handles)
	GLuint pyramid_BO_array[2];
	
	// pyramid vertex Data
		// altogether 5 vertices and 3 coordinates each for x, y and z
		GLfloat pyramid_vertices[5*3] = {
			-1.0f, 0.0f, 1.0f,  // 0
			 1.0f, 0.0f, 1.0f,  // 1
			 0.0f, 1.0f, 0.0f,  // 2
			-1.0f, 0.0f,-1.0f,  // 3
			 1.0f, 0.0f,-1.0f   // 4
		};

		// altogether 6 triangles to 3 shared vertices each
		GLuint pyramid_indices[6*3] = {
			// front
			0, 1, 2,
			// right
			1, 4, 2,
			// back
			4, 3, 2,
			// left
			3, 0, 2,
			// bottom (front right)
			0, 1, 4, 
			// bottom (back left)
			0, 3, 4
		};

	// Create our Vertex Array Object
	glGenVertexArrays(1, &VAO);
	// Bind our Vertex Array Object so we can use it
	glBindVertexArray(VAO);

		// Generate 2 Buffer Objects (VBO & IBO)
		glGenBuffers(2, pyramid_BO_array);
		glBindBuffer(GL_ARRAY_BUFFER, pyramid_BO_array[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);

		// Once VB is complete, tell shader what it is
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// index buffer for our triangle
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_BO_array[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), pyramid_indices, GL_STATIC_DRAW);

	// Unbind our triangle Vertex Array Object
	glBindVertexArray(0);
}

void process_mouse(int button, int state, int x, int y);

void init_scene()
{
	// setup shader
	shaderProgram = CompileShaders(pVS, pFS);

	// setup matrices
	modelMatrix = glm::mat4(1.0f);
}

void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    // continuously called idle
    glutIdleFunc(RenderSceneCB);
	glutMouseFunc(process_mouse);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH |GLUT_RGBA);
    glutInitWindowSize(window_width, window_heigth);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("draw objects");

    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }

	init_scene();

	init_resources();

    glutMainLoop();

    return 0;
}


// -------------------User Interaction--------------------


void process_mouse(int button, int state, int x, int y) 
{
	if(button == 3)
	{
		wsc += 0.1;

		cout.precision(2);
		cout << "4  " << "wsc: " << fixed << wsc << endl;
	}
	else if(button == 4)
	{
		wsc -= 0.1;

		cout.precision(2);
		cout << "4  " << "wsc: " << fixed << wsc << endl;
	}

}