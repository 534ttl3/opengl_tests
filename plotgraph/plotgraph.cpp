/* 06 - translation transform */
// some basic headers
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

// shader init
#include "init_shaders.h"
using namespace std;


const int ncalcpoints = 9000;
float step = 0.0005;
float zoom = 1.0;
float zoomspeed = 0.01;
float xstreckf = 1.0;
float ystreckf = 1.0;
int sinScale = 0.00;
int sth = 0.0;

// Vertex Buffer Object
GLuint VBO, qVBO;
GLuint      qCB;
GLuint CBO;
GLuint hSP;

GLuint axisVBO[3];
GLuint axisCBO[3];

GLuint fVBO;
GLuint fCBO;

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
			                                                                        \n\
layout (location = 0) in vec3 Position;		                                        \n\
layout (location = 1) in vec4 vColor;   									        \n\
out vec4 Color;																		\n\
																					\n\
void main()                                                                         \n\
{                                                                                   \n\
	Color = vColor;                                                                 \n\
	gl_Position = vec4(Position, 1.0);	                                            \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
in vec4 Color;                                                                      \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
	                                                                                \n\
    FragColor = Color;																\n\
}";


void processMouse(int button, int state, int x, int y) 
{
	if(button == 3)
	{
		if(!(zoom <= 0.01))
			zoom -= zoomspeed;
	}
	else if(button == 4)
	{
		zoom += zoomspeed;
	}

}

void process_SHIFT_ALT_CTRL(unsigned char key, int x, int y) 
{
    // Press ALT or  SHIFT or  CTRL in combination with other keys.

	if(key == 'a')
		sth = 1;
	else if(key == 'b')
		sth = 2;

	int id = 0;

    int mod = glutGetModifiers();

    if (mod != 0) //ALT=4  SHIFT=1  CTRL=2
    {      
          switch(mod)
          {
             case GLUT_ACTIVE_SHIFT :  printf("SHIFT "); id = 1; break;
             case GLUT_ACTIVE_CTRL  :  printf("CTRL ");  id = 2; break;
             case GLUT_ACTIVE_ALT   :  printf("ALT ");   id = 3; break;
			 mod=0;
			 break;
          }

		  if((int) key == 1)
		  {
			  cout << "A" << endl;
			  zoomspeed += 0.01;
		  }

		  if((int) key == 4)
		  {  
			  cout << "D" << endl;
			  if(zoomspeed >= 0.0)
				  zoomspeed -= 0.01;
		  }
		  	
		  if((int) key == 24 && id == 2)
		  {  
			  cout << "X+" << endl;
				  xstreckf += 0.02;
		  }

		  if((int) key == 'X' && id == 1)
		  {  
			  cout << "X-" << endl;
			  if(xstreckf >= 0.021)
				  xstreckf -= 0.02;
		  }

		  if((int) key == 25 && id == 2)
		  {  
			  cout << "Y+" << endl;
				  ystreckf += 0.02;
		  }

		  if((int) key == 'Y' && id == 1)
		  {  
			  cout << "Y-" << endl;
			  if(xstreckf >= 0.021)
				  ystreckf -= 0.02;
		  }
	}
}

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// x axis
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, axisCBO[0]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);


	// y axis
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, axisCBO[1]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINES, 0, 2);

	
	// process graph

	if(sth == 1)
		xstreckf *= 1.02;
	else if(sth == 2)
		xstreckf /= 1.05;
	// vertices
	GLfloat coords[3*ncalcpoints];

	int i = 0; // walk indices
	float x = -1; // left extreme borde
	for(i = 0; i < 3*ncalcpoints; i += 3)
	{
		coords[i]   = x *( xstreckf)/ zoom;									// x coordinate
		coords[i+1] = ( x*sinf(1/x) ) *ystreckf / zoom;						// y coordinate
		coords[i+2] = -0.1f;												// z coordinate
		x += step;
	}

	glGenBuffers(1, &fVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);

	// color
	// already processed in CreateVertexBuffer()


	// draw graph


	glBindBuffer(GL_ARRAY_BUFFER, fVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, fCBO);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_LINE_STRIP, 0, ncalcpoints);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);


    glutSwapBuffers();
}

static void CreateVertexBuffer()
{	
	// axes
	GLfloat xaxis[2*3] = {
		1.0f, 0.0f, 0.0f,
	   -1.0f, 0.0f, 0.0f
	};

	GLfloat yaxis[2*3] = {
		0.0f, 1.0f, 0.0f,
	    0.0f,-1.0f, 0.0f
	};
	
	glGenBuffers(1, &axisVBO[0]);
	glGenBuffers(1, &axisVBO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, axisVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xaxis), xaxis, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, axisVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yaxis), yaxis, GL_STATIC_DRAW);


	GLfloat xaxisc[2*4] = {
		1.0f, 0.0f, 0.0f, 1.0,
	    1.0f, 0.0f, 0.0f, 1.0
	};

	GLfloat yaxisc[2*4] = {
		0.0f, 1.0f, 0.0f, 1.0,
	    0.0f, 1.0f, 0.0f, 1.0
	};

	glGenBuffers(1, &axisCBO[0]);
	glGenBuffers(1, &axisCBO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, axisCBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xaxisc), xaxisc, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, axisCBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yaxisc), yaxisc, GL_STATIC_DRAW);

	// setup graph color

	GLfloat fcolor[ncalcpoints*4];

	for(int i = 0; i < ncalcpoints*4; i += 4)
	{
		fcolor[i]   = 1.0f; fcolor[i+1] = 1.0f;
		fcolor[i+2] = 1.0f; fcolor[i+3] = 1.0f;
	}

	glGenBuffers(1, &fCBO);
	glBindBuffer(GL_ARRAY_BUFFER, fCBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fcolor), fcolor, GL_STATIC_DRAW);
}

static void init()
{
	//...
}

static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    // continuously called idle
    glutIdleFunc(RenderSceneCB);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(process_SHIFT_ALT_CTRL);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH |GLUT_RGBA);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("draw objects");

	init();

    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();
    CompileShaders(pVS, pFS);

    glutMainLoop();

    return 0;
}