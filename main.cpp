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
#include <iostream>
#include <math.h>
#include "marble.h"
#include <vector> 

using namespace std; 

int gWidth = 600;
int gHeight = 600;

void reshape(int w, int h) //used to handle window resizing
{
    gWidth  = w;
    gHeight = h;

    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity(); 
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h); 
}

void display(void){ //display funtion
    glClear(GL_COLOR_BUFFER_BIT);


    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void init() {
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); //double buffering
    glutInitWindowSize(gWidth, gHeight);
    glutCreateWindow("Particle System");
    glClearColor(0.0,0.0,0.0,0);
    glColor3f(0.0,1.0,1.0);
}

int main (int argc,char** argv){ //main function

  
    glutInit(&argc,argv);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return (0);
}