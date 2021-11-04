#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>	
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "mathLib2D.h"
#include <vector>
#include <random>

int gwidth = 700;
int gheight = 700;

//some initialization
void init(void)
{	
	glEnable(GL_LIGHT0);	//enable first light
	glEnable(GL_LIGHT1);	//enable second light
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-2, 2, -2, 2, -2, 2);
	gluPerspective(45, 1, 1, 100);
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    
	
	glutInitWindowSize(gwidth, gheight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Particle Fountain");	//creates the window


	glEnable(GL_DEPTH_TEST);
    
    std::cout<<"Hello"<<endl;
    
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
    
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}