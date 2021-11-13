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
#include "terrain.h"
#include "player.h"
#include "particle.h"
#include <math.h>
#include <vector>
#include <random>
#include <iostream>

using namespace std; 

float verts[8][3] = { {-1,-1,1}, {-1,1,1}, {1,1,1}, {1,-1,1}, {-1,-1,-1}, {-1,1,-1}, {1,1,-1}, {1,-1,-1} };

std::random_device rd; 
std::mt19937 gen(rd()); 
std::uniform_real_distribution<float> dis(0.0, 0.5);

float camPos[] = {0, 60, 42}; 
float upPos[] = {0, 1, 0};



float ambMat[7][4] = {{0.5, 0.5, 0.5, 1}, {0.2, 0.6, 0.2, 1}, {0.6, 0.2, 0.2, 1}, {0.2, 0.2, 0.6, 1},{ 0.1f, 0.18725f, 0.1745f, 0.8f },{ 0.05f,0.05f,0.0f,1.0f },{ 0.2295f, 0.08825f, 0.0275f, 1.0f }};
float diffMat[7][4] = {{0.5, 0, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 0, 1, 0},{0.396f, 0.74151f, 0.69102f, 0.8f },{0.5f,0.5f,0.4f,1.0f},{0.5508f, 0.2118f, 0.066f, 1.0f }};
float specMat[7][4] = {{0, 0.5, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 1, 1, 0},{0.297254f, 0.30829f, 0.306678f, 0.8f},{0.7f,0.7f,0.04f,1.0f},{0.580594f, 0.223257f, 0.0695701f, 1.0f }};

float scaley = 0;
float scalex = 0;

vector <grid> Grids;

/* LIGHTING */
float light_pos[4] = {5, 5, 5, 0};
float amb[4] = {1, 1, 1, 1};
float diff[4] = {1, 1, 1, 1};
float spec[4] = {1, 1, 1, 1};

float light_pos2[4] = {-5, -5, -5, 0};
float amb2[4] = {1, 0.5, 0.5, 1};
float diff2[4] = {1, 0.5, 0.5, 1};
float spec2[4] = {1, 0.5, 0.5, 1};

/* -1 when partcam is off, non negative otherwise */
int partcam = -1;

vector<Particle> parts;

// return a random float in [0,1)
float randf()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void drawAxis(float x, float z){
    glPushMatrix();
	glTranslatef(x,0,z);
    glLineWidth(3);
    glBegin(GL_LINES);


    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 5.0, 0.0);


    glEnd();
    glPopMatrix();
}

player Player1(1,2,0,-18.5,18.5);
player Player2(2,2,0,18.5,18.5);
player Player3(3,2,0,-18.5,-18.5);
player Player4(4,2,0,18.5,-18.5);


int GetNumber(float x, float z){	//return the current number of grid using x and z
	return ((x + 19.5) * 40 + (19.5 - z));
}

/* draw firework at position (x, 0, z) */
void firework(float x, float z)
{
	Particle newPart(x, 0, z, 0, 1, 0);
	newPart.age = 60;
	newPart.speed = 0.1;
	newPart.size = 0.3;
	parts.push_back(newPart);
}

/* firework explosion*/
void firework1(float dx, float dy, float dz, int mat)
{
	for (int i = 0; i < 20; i++)
	{
		float x = randf() - 0.5;
		float z = randf() - 0.5;
		float y = randf() - 0.5;
		float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		Particle newPart(dx, dy, dz, x / length, y / length, z / length);
		newPart.age = 60;
		newPart.speed = 1;
		newPart.size = 0.2;
		newPart.mat = mat;
		parts.push_back(newPart);
	}
}

/* draw fountain at position (px, py, pz) */
void fountain(int px = 0, int py = 0, int pz = 0)
{
	for (int i = 0; i < 30; i++)
	{
		float x = randf() - 0.5;
		float z = randf() - 0.5;
		float y = 20;
		float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		Particle newpart(px, py, pz, x / length, y / length, z / length);
		newpart.size = 0.2;
		newpart.age = -3*i;
		parts.push_back(newpart);
	}
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

float mx = -19.5;
float mz = 19.5;
//create 40x40 floor centering at origin (0,0,0)
void GenFloor(){ 
	grid newGrid;

	for (int i = 1; i <= 1600; i++){
		float H = dis(gen);
    	int mat = rand()%7;
		newGrid = grid(i,mat,H,mx,mz);
		Grids.push_back(newGrid);

		if (mx < 20 && mz > -20){
			mz -=1;
		}
		if (mx < 20 && mz < -20){
			mx +=1;
			mz = 19.5;
		}
		//std::cout<<mx<<" "<<mz<<endl;
	}
	//std::cout<<Grids.size()<<" floors generated!!!"<<endl;
}

//Used to draw the grid floor
void drawgridfloor(){
	for (unsigned int i = 0; i <= Grids.size(); i++){
		if (i == GetNumber(Player1.basex,Player1.basez)){
			Grids[i].mat = Player1.mat;
		}
		if (i == GetNumber(Player2.basex,Player2.basez)){
			Grids[i].mat = Player2.mat;
		}
		if (i == GetNumber(Player3.basex,Player3.basez)){
			Grids[i].mat = Player3.mat;
		}
		if (i == GetNumber(Player4.basex,Player4.basez)){
			Grids[i].mat = Player4.mat;
		}
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[Grids[i].mat]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);
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
		case 'a':
		firework(rand() % 10, rand() % 10);
		break;
	case 'p':
		partcam = -1;
		break;
	case 'f':
	case 'F':
		fountain((randf()-0.5)*15,0,(randf()-0.5)*15);
		break;

	case 'r':
	case 'R':
		while (int i = parts.size() > 6)
		{
			parts.pop_back();
		}
		break;
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
		partcam = key - 49;
		break;
	}		
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y){
	float radius = sqrt(2 * pow(30,2));// TOBE FIXED 1. Special key rotation is not smooth at first use.
	switch (key)
	{
	//camera rotate about y axis
	case GLUT_KEY_LEFT:
		scaley = scaley + 0.1;
		camPos[0] = radius * sin(scaley);
		camPos[2] = radius * cos(scaley);
		//std::cout<<camPos[0]<<" "<<camPos[1]<<" "<<camPos[2]<<endl;
		break;
	case GLUT_KEY_RIGHT:
		scaley = scaley - 0.1;
		camPos[0] = radius * sin(scaley);
		camPos[2] = radius * cos(scaley);
		//std::cout<<camPos[0]<<" "<<camPos[1]<<" "<<camPos[2]<<endl;
		break;
	//camera rotate about x axis
	case GLUT_KEY_UP:
		scalex = scalex + 0.1;
		camPos[1] = radius * sin(scalex);
		camPos[2] = radius * cos(scalex);
		//std::cout<<camPos[0]<<" "<<camPos[1]<<" "<<camPos[2]<<endl;
		break;
	case GLUT_KEY_DOWN:
		scalex = scalex - 0.1;
		camPos[1] = radius * sin(scalex);
		camPos[2] = radius * cos(scalex);
		//std::cout<<camPos[0]<<" "<<camPos[1]<<" "<<camPos[2]<<endl;
		break;
	}
	glutPostRedisplay();
}

/* initial 6 permanent particles */
void initParts()
{
for (int i = 0; i < 6; i++)
	{
		float px = (randf() - 0.5) * 20;
		float pz = (randf() - 0.5) * 20;
		float py = 0;
		float x = randf() - 0.5;
		float z = randf() - 0.5;
		float y = 0;
		float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		Particle newpart(px, py, pz, x / length, y / length, z / length);
		newpart.is_permanent = true;
		newpart.size = 0.4;
		newpart.speed = 0.2;
		parts.push_back(newpart);
	}
}


void init(void)
{	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
	// Initial 6 permanent particles.
	initParts();
}

/* draw particles */
void drawparts()
{
	glPushMatrix();
	glTranslatef(0, 1, 0);
	for (int i = 0; i < parts.size(); ++i)
	{
		if (parts[i].age < 0) continue;
		int mat = parts[i].mat;
		glPushMatrix();
		glTranslatef(parts[i].position[0], parts[i].position[1], parts[i].position[2]);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[parts[i].mat]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);

		glColor3f(ambMat[mat][0], ambMat[mat][1], ambMat[mat][2]);
		glutSolidSphere(parts[i].size, 16, 16);

		glPopMatrix();
	}
	glPopMatrix();
}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Particle camera is on
	if (partcam != -1)
	{
		gluLookAt(parts[partcam].position[0] - parts[partcam].direction[0], 25, parts[partcam].position[2] - parts[partcam].direction[2],
		parts[partcam].position[0], 3, parts[partcam].position[2], upPos[0], upPos[1], upPos[2]);
	}
	else
	{
		gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, upPos[0], upPos[1], upPos[2]);
	}
	glColor3f(1, 1, 1);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);

    //drawBox(origin, 1, 0.5, 1);
	//drawBox(origin1,1,0.5,1);
    drawAxis(-18.5,18.5);
	drawAxis(18.5,18.5);
	drawAxis(-18.5,-18.5);
	drawAxis(18.5,-18.5);
	drawgridfloor();

	drawparts();

	glutSwapBuffers();
}

/* Delete a particle when its age greater than default life duration */
void deleteParticle(int i)
{
	parts[i] = parts[parts.size() - 1];
	parts.pop_back();
}

/* move all particles */
void moveAll()
{
	for (int i = 0; i < parts.size(); i++)
	{
		// Negative age to delay particle creation, so particles with negative age won't be moved
		if (parts[i].age < 0) continue;
		parts[i].position[0] += parts[i].direction[0] * parts[i].speed;
		parts[i].position[1] += parts[i].direction[1] * parts[i].speed;
		parts[i].position[2] += parts[i].direction[2] * parts[i].speed;
		float x, y, z, length;
		x = parts[i].direction[0] * parts[i].speed;
		y = parts[i].direction[1] * parts[i].speed - 0.1;
		z = parts[i].direction[2] * parts[i].speed;
		length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		parts[i].direction[0] = x / length;
		parts[i].direction[1] = y / length;
		parts[i].direction[2] = z / length;
		// Change direction if it reaches boundary.
		if (parts[i].position[0] > 20 || parts[i].position[0] < -20){
			parts[i].direction[0] = -parts[i].direction[0];
		}
		if (parts[i].position[2] > 20 || parts[i].position[2] < -20){
			parts[i].direction[2] = -parts[i].direction[2];
		}
		// Change direction if it reaches the floor.
		if (parts[i].position[1] < 0 && parts[i].direction[1] < 0)
		{
			parts[i].direction[1] = -parts[i].direction[1];
			// Decrease its speed unless it is permanent paritcle.
			if (!parts[i].is_permanent)
				parts[i].speed *= 0.7;
		}
	}
}

/* FPS function - 60 FPS per second */
void FPS(int val)
{
	moveAll();
	glutPostRedisplay();
	for (int i = 0; i < parts.size(); i++)
	{
		if (!parts[i].is_permanent)
			parts[i].age++;
		if (parts[i].age >= Particle::DEFAULT_LIFE_DURATION)
		{
			if (parts[i].size > 0.3)
			{
				firework1(parts[i].position[0], parts[i].position[1], parts[i].position[2], parts[i].mat);
			}
			deleteParticle(i);
		}
	}
	glutTimerFunc(17, FPS, 0);
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("marble war");	//creates the window
	
	GenFloor();
	glutDisplayFunc(display);	//registers "display" as the display callback function
	//std::cout<<Grids[800].getNumber()<<endl;	
	glutKeyboardFunc(keyboard);
    glutSpecialFunc(SpecialKey);
	glutTimerFunc(17, FPS, 0);

	glEnable(GL_DEPTH_TEST);
    
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
    
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}
