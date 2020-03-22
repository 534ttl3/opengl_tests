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
GLuint attrib_Position, attrib_Color;

// Vertex Buffer Object
GLuint triangle_VBO;


static const char* pVS = "                                                          \n\
#version 400                                                                        \n\
			                                                                        \n\
layout (location = 0) in vec3 Position;		                                        \n\
										        \n\
out vec4 vColor;																	\n\
uniform mat4 projectionMatrix;														\n\
uniform mat4 viewMatrix;															\n\
uniform mat4 modelMatrix;															\n\
																					\n\
void main()                                                                         \n\
{                                                                                   \n\
	vColor = vec4(Position, 1.0);                                                                 \n\
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
		glm::vec3(0, 0, wsc),		
		glm::vec3(0, 0, 0),		// look at this point
		glm::vec3(0, 1, 0)		// some kind of turn the camera
		);
	static int a = 0;
	modelMatrix = glm::rotate(modelMatrix, 1.0f, glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, 1.0f, glm::vec3(1, 0, 1));

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

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 342*3);

	glDisableVertexAttribArray(attrib_Position);

    glutSwapBuffers();
}

void init_resources()
{
	// process sphere

	const float DegreesToRadians = 3.14159265358979323846 / 180.0; // M_PI = 3.14159...
	glm::vec3 quad_data[342]; // 8 rows of 18 quads
	int k = 0;
	for(float phi = -80.0; phi <= 80.0; phi += 20.0)
	{
		float phir = phi*DegreesToRadians;
		float phir20 = (phi + 20.0)*DegreesToRadians;
		for(float theta = -180.0; theta <= 180.0; theta += 20.0)
		{
			float thetar = theta*DegreesToRadians;
			quad_data[k] = glm::vec3(sin(thetar)*cos(phir), cos(thetar)*cos(phir), sin(phir));
			k++;
			quad_data[k] = glm::vec3(sin(thetar)*cos(phir20), cos(thetar)*cos(phir20), sin(phir20));
			k++;
		}
	}

	glGenBuffers(1, &triangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

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