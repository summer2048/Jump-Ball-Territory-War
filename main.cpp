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
#include "marble.h"
#include "terrain.h"
#include "player.h"
#include <math.h>
#include <vector>
#include <random>

using namespace std; 

float verts[8][3] = { {-1,-1,1}, {-1,1,1}, {1,1,1}, {1,-1,1}, {-1,-1,-1}, {-1,1,-1}, {1,1,-1}, {1,-1,-1} };

std::random_device rd; 
std::mt19937 gen(rd()); 
std::uniform_real_distribution<float> dis(0.0, 0.5);

float pos[] = {0,1,0};
float rot[] = {0, 0, 0};
float headRot[] = {0, 0, 0};
float camPos[] = {5, 5, 10};

vector <grid> Grids;

void drawAxis(){
    glPushMatrix();
	glTranslatef(-9,0,9);
    glLineWidth(2);
    glBegin(GL_LINES);


    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 5.0, 0.0);


    glEnd();
    glPopMatrix();
}

/* drawPolygon - takes 4 indices and an array of vertices
 *   and draws a polygon using the vertices indexed by the indices
 */
void drawPolygon(int a, int b, int c, int d, float v[8][3]){
	glBegin(GL_POLYGON);
		glVertex3fv(v[a]);
		glVertex3fv(v[b]);
		glVertex3fv(v[c]);
		glVertex3fv(v[d]);
	glEnd();
}

/* cube - takes an array of 8 vertices, and draws 6 faces
 *  with drawPolygon, making up a box
 */
void cube(float v[8][3])
{
//  faces[6][4];
//	glNormal3f(0,1,0);
	drawPolygon(0, 3, 2, 1, v);


	drawPolygon(1, 0, 4, 5, v);

	drawPolygon(5, 1, 2, 6, v);
	
	drawPolygon(2, 3, 7, 6, v);
	
	drawPolygon(6, 5, 4, 7, v);

	drawPolygon(4, 0, 3, 7, v);
}

/* drawBox - takes centre point, width, height and depth of a box,
 *  calculates its corner vertices, and draws it with the cube function
 */
void drawBox(float* c, float w, float h, float d)
{
	float vertices[8][3] = { {c[0]-w/2, c[1]-h/2, c[2]+d/2},
							 {c[0]-w/2, c[1]+h/2, c[2]+d/2},
							 {c[0]+w/2, c[1]+h/2, c[2]+d/2},
							 {c[0]+w/2, c[1]-h/2, c[2]+d/2}, 
							 {c[0]-w/2, c[1]-h/2, c[2]-d/2}, 
							 {c[0]-w/2, c[1]+h/2, c[2]-d/2}, 
							 {c[0]+w/2, c[1]+h/2, c[2]-d/2},
							 {c[0]+w/2, c[1]-h/2, c[2]-d/2} };

	cube(vertices);
}

float mx = -9.5;
float mz = 9.5;
void GenFloor(){
	grid newGrid;

	for (int i = 1; i <= 400; i++){
		float H = dis(gen);
    	colour c = colour(rand() %2,rand() %2,rand() %2);
		newGrid = grid(c,H,mx,mz);
		Grids.push_back(newGrid);

		if (mx < 10 && mz > -10){
			mz -=1;
		}
		if (mx < 10 && mz < -10){
			mx +=1;
			mz = 9.5;
		}
		//std::cout<<mx<<" "<<mz<<endl;
	}
	//std::cout<<Grids.size()<<" floors generated!!!"<<endl;
}

void drawgridfloor(){

	for (unsigned int i = 0; i < Grids.size(); i++){
		glColor3f(get<0>(Grids[i].color),get<1>(Grids[i].color),get<2>(Grids[i].color));
		float origin[3] = {Grids[i].mx,0,Grids[i].mz};
		drawBox(origin,1,Grids[i].Height,1);
	}	
	//glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
		case 'q':
		case 27:
			exit (0);
			break;			
	}
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y){
	switch(key){
    	case GLUT_KEY_UP: //'up' key to increment eye y position
			camPos[1]++;
			break;
		case GLUT_KEY_DOWN: //'down' key to decrease eye y position
			camPos[1]--;
			break;
		case GLUT_KEY_LEFT: //'left' key to increment eye x position
			camPos[0]++;
			break;
		case GLUT_KEY_RIGHT: //'right' key to decrease eye x position
			camPos[0]--;
			break;	
	}
	glutPostRedisplay();
}



void init(void)
{	
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	glOrtho(-5, 5, -5, 5, -5, 80);
	gluPerspective(80, 1, 1, 100);
}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);

    //drawBox(origin, 1, 0.5, 1);
	//drawBox(origin1,1,0.5,1);
    //drawAxis();
	drawgridfloor();

	glutSwapBuffers();
	
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	cout << 'hello' << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("marble war");	//creates the window
	
	GenFloor();
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
    glutSpecialFunc(SpecialKey);

	glEnable(GL_DEPTH_TEST);
    
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
    
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}
