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
GLfloat wsc;

// shader program
GLuint shaderProgram;

// martrices
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;		// Store the view matrix
glm::mat4 modelMatrix;		// Store the model matrix

// attributes
GLuint attrib_Position, attrib_Color;

// Vertex Buffer Object
GLuint triangle_VBO_array[2];


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
	vColor = Color;                                                                 \n\
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
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glUseProgram(shaderProgram);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create our perspective projection matrix
	projectionMatrix = glm::perspective(100.0f, (float)window_width / (float)window_heigth, 0.1f, 100.0f);

	viewMatrix = glm::lookAt(
		glm::vec3(0, 3, 5),		
		glm::vec3(0, 0, 0),		// look at this point
		glm::vec3(0, 1, 0)		// some kind of turn the camera
		);
	static int a = 0;
	modelMatrix = glm::rotate(modelMatrix, 1.0f, glm::vec3(0, 1, 0));

	int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix"); 
	int viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix"); 

	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); 

	// wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// draw triangle

	glEnableVertexAttribArray(attrib_Position);
	glEnableVertexAttribArray(attrib_Color);

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO_array[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO_array[1]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 4*3);

	glDisableVertexAttribArray(attrib_Color);
	glDisableVertexAttribArray(attrib_Position);

    glutSwapBuffers();
}

void init_resources()
{
	GLfloat triangle_vertices[4*3*3] = {
		// front
	 0.0f, 1.f, 0.0f,
	 -1.0f, -1.0f, 1.0f, 
	 1.0f, -1.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,
	  -1.0f, -1.0f, 1.0f,
	  0.0f, -1.0f, -1.0f,

	  0.0f, 1.0f, 0.0f,
	   0.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, 1.0f,

	   -1.0f, -1.0f, 1.0f,
	   0.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, 1.0f
	};

	GLfloat triangle_colors[4*3*4] = {
	 1.0, 0.0, 0.0, 1.0, 
	 0.0, 1.0, 0.0, 1.0, 
	 0.0, 0.0, 1.0, 1.0,

	 1.0, 0.0, 0.0, 1.0, 
	 0.0, 1.0, 0.0, 1.0, 
	 0.0, 0.0, 1.0, 1.0,

	 1.0, 0.0, 0.0, 1.0, 
	 0.0, 1.0, 0.0, 1.0, 
	 0.0, 0.0, 1.0, 1.0,

	 1.0, 0.0, 0.0, 1.0, 
	 0.0, 1.0, 0.0, 1.0, 
	 0.0, 0.0, 1.0, 1.0
	};


	glGenBuffers(2, triangle_VBO_array);

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO_array[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO_array[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);

	attrib_Position = glGetAttribLocation(shaderProgram, "Position");
	attrib_Color    = glGetAttribLocation(shaderProgram, "Color");
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