#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "terrain.h"
#include "player.h"
#include "particle.h"
#include "object.h"
#include "ObjectLoader.h"
#include <math.h>
#include <vector>
#include <random>
#include <map>
#include <iostream>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0, 0.5);


float upPos[] = {0, 1, 0};

GLubyte *img_1;
int Width1, Height1, Max1;
GLubyte *Welcome_img;
int Width2, Height2, Max2;
GLubyte* CrossSkill_img;
int Width3, Height3, Max3;
unsigned int Texture[3];
ObjectLoader obj1;
map<string, ObjectLoader> list;

int gameSpeed = 1;


float ambMat[7][4] = {{0, 0, 0, 1}, {0.2, 0.6, 0.2, 1}, {0.6, 0.2, 0.2, 1}, {0.2, 0.2, 0.6, 1}, {0.2295f, 0.08825f, 0.0275f, 1.0f}, {0.5, 0.4, 0.3, 0.2}, {0.05f, 0.05f, 0.0f, 1.0f}};
float diffMat[7][4] = {{0.5, 0, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 0, 1, 0}, {0.5508f, 0.2118f, 0.066f, 1.0f}, {0.396f, 0.74151f, 0.69102f, 0.8f}, {0.5f, 0.5f, 0.4f, 1.0f}};
float specMat[7][4] = {{0, 0.5, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}, {0.580594f, 0.223257f, 0.0695701f, 1.0f}, {0.297254f, 0.30829f, 0.306678f, 0.8f}, {0.7f, 0.7f, 0.04f, 1.0f}};
/*camera scale*/
float radius = 65;
float scaley = 44;
float scalex = 90;
float pi = 3.1415926536f;
float camPos[] = { (float)(radius * cos(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0))) , (float)(radius * cos(scaley * (pi / 180.0))) , (float)(radius * sin(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0))) };

int MouseX = 0;			   //Mouse position X
int MouseY = 0;			   //Mouse position Y
bool LeftButton = false;   // track left mouse status
bool RightButton = false;  // track right mouse status
bool middleButton = false; //track middle mouse status
bool startgame = false;
bool freemode = false;
bool endgame = false;
bool heart1 = true;
bool heart2 = true;
bool heart3 = true;
bool heart4 = true;
vector<grid> Grids;
int angle = 0;	//heart angle
float heartx = 0.0;
float heartz = 0.0;
/* LIGHTING */
float light_pos[4] = {5, 5, 5, 0};
float amb[4] = {1, 1, 1, 1};
float diff[4] = {1, 1, 1, 1};
float spec[4] = {1, 1, 1, 1};

float light_pos2[4] = {-5, -5, -5, 0};
float amb2[4] = {1, 0.5, 0.5, 1};
float diff2[4] = {1, 0.5, 0.5, 1};
float spec2[4] = {1, 0.5, 0.5, 1};


vector<Particle> parts;
vector<object> objects;

/* Object ray pick data*/
int picked_object = -1;
double *m_start = new double[3];
double *m_end = new double[3];
double *Rd = new double[3];

// return a random float in [0,1)
float randf()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
struct Handler
{
	unsigned int mLeft, mRight, mTop, mBottom;
	void (*mHandlerFunc)();

	bool isInBounds(unsigned int x, unsigned int y)
	{
		if (x <= mRight && x >= mLeft && y <= mTop && y >= mBottom)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void handleClickAt(unsigned int x, unsigned int y)
	{
		if (isInBounds(x, y))
		{
			mHandlerFunc();
		}
	}

	void drawBoxVertices()
	{
		glVertex3f(mLeft, mTop, 1);
		glVertex3f(mLeft, mBottom, 1);
		glVertex3f(mRight, mTop, 1);
		glVertex3f(mRight, mBottom, 1);
		glVertex3f(mLeft, mTop, 1);
		glVertex3f(mRight, mTop, 1);
		glVertex3f(mLeft, mBottom, 1);
		glVertex3f(mRight, mBottom, 1);
	}
};

struct InteractionHandler
{
	std::vector<Handler *> mHandlers;

	void leftClickDown(int x, int y)
	{
		for (Handler *handler : mHandlers)
		{
			handler->handleClickAt(x, y);
		}
	}

	void drawHandlers()
	{
		glColor3f(1, 1, 1);
		glLineWidth(2);
		glBegin(GL_LINES);
		for (Handler *handler : mHandlers)
		{
			handler->drawBoxVertices();
		}
		glEnd();
	}

	void addHandler(Handler *handler)
	{
		mHandlers.push_back(handler);
	}
};

InteractionHandler mouseHandler;
InteractionHandler startHandler;
GLubyte *LoadPPM(char *file, int *width, int *height, int *max)
{
	GLubyte *img;
	FILE *fd;
	int n, m;
	int k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;

	fd = fopen(file, "r");
	fscanf(fd, "%[^\n] ", b);
	if (b[0] != 'P' || b[1] != '3')
	{
		printf("%s is not a PPM file!\n", file);
		exit(0);
	}
	//printf("%s is a PPM file\n", file);
	fscanf(fd, "%c", &c);
	while (c == '#')
	{
		fscanf(fd, "%[^\n] ", b);
		printf("%s\n", b);
		fscanf(fd, "%c", &c);
	}
	ungetc(c, fd);
	fscanf(fd, "%d %d %d", &n, &m, &k);

	//printf("%d rows  %d columns  max value= %d\n",n,m,k);

	nm = n * m;

	img = (GLubyte *)(malloc(3 * sizeof(GLuint) * nm));

	s = 255.0 / k;

	for (i = 0; i < nm; i++)
	{
		fscanf(fd, "%d %d %d", &red, &green, &blue);
		img[3 * nm - 3 * i - 3] = red * s;
		img[3 * nm - 3 * i - 2] = green * s;
		img[3 * nm - 3 * i - 1] = blue * s;
	}

	*width = n;
	*height = m;
	*max = k;

	return img;
}

player Player1(1, 2, 0, -18.5, 18.5);
player Player2(2, 2, 0, 18.5, 18.5);
player Player3(3, 2, 0, -18.5, -18.5);
player Player4(4, 2, 0, 18.5, -18.5);

int GetNumber(float x, float z)
{ //return the current number of grid using x and z
	return ((x + 19.5) * 40 + (19.5 - z));
}

/* draw firework at position (x, 0, z) */
void firework(float x, float z, int mat)
{
	Particle newPart(x, 0, z, 0, 1, 0);
	newPart.age = 60;
	newPart.speed = 0.1;
	newPart.size = 0.3;
	newPart.is_firework = true;
	newPart.mat = mat;
	parts.push_back(newPart);
}

void Pastrybags(float x, float z){
	Particle newPart(x,15, z, 0, -1, 0);
	newPart.age = 120;
	newPart.speed = 0.5;
	newPart.size = 0.15;
	newPart.mat = rand()%4 + 1;
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
void fountain(int px = 0, int py = 0, int pz = 0, int mat = 0)
{
	for (int i = 0; i < 30; i++)
	{
		float x = (randf() - 0.5) * 0.5;
		float z = (randf() - 0.5) * 0.5;
		float y = 20;
		float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		Particle newpart(px, py, pz, x / length, y / length, z / length);
		newpart.size = 0.2;
		newpart.speed = 0.5;
		newpart.age = -3 * i;
		newpart.mat = mat;
		parts.push_back(newpart);
	}
}

/* drawPolygon - takes 4 indices and an array of vertices
 *   and draws a polygon using the vertices indexed by the indices
 */
void drawPolygon(int a, int b, int c, int d, float v[8][3])
{
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3fv(v[a]);

	glTexCoord2f(0, 1);
	glVertex3fv(v[b]);

	glTexCoord2f(1, 1);
	glVertex3fv(v[c]);

	glTexCoord2f(1, 0);
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

/* drawBox */
void drawBox(float *c, float w, float h, float d)
{
	float vertices[8][3] = {{c[0] - w / 2, c[1] - h / 2, c[2] + d / 2},
							{c[0] - w / 2, c[1] + h / 2, c[2] + d / 2},
							{c[0] + w / 2, c[1] + h / 2, c[2] + d / 2},
							{c[0] + w / 2, c[1] - h / 2, c[2] + d / 2},
							{c[0] - w / 2, c[1] - h / 2, c[2] - d / 2},
							{c[0] - w / 2, c[1] + h / 2, c[2] - d / 2},
							{c[0] + w / 2, c[1] + h / 2, c[2] - d / 2},
							{c[0] + w / 2, c[1] - h / 2, c[2] - d / 2}};

	cube(vertices);
}

float mx = -19.5;
float mz = 19.5;
//create 40x40 floor centering at origin (0,0,0)
void GenFloor()
{
	grid newGrid;

	for (int i = 1; i <= 1600; i++)
	{
		float H = dis(gen);
		// int mat = rand()%7;
		newGrid = grid(i, 0, H, mx, mz);
		Grids.push_back(newGrid);

		if (mx < 20 && mz > -20)
		{
			mz -= 1;
		}
		if (mx < 20 && mz < -20)
		{
			mx += 1;
			mz = 19.5;
		}
	}
}

//Used to draw the grid floor
void drawgridfloor()
{
	for (unsigned int i = 0; i <= Grids.size(); i++)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[Grids[i].mat]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);
		float origin[3] = {Grids[i].mx, 0, Grids[i].mz};
		drawBox(origin, 1, Grids[i].Height, 1);
	}
}

void Intersectiontest()
{	if (startgame){
	double projection[16];
	double modelview[16];
	int viewport[4];
	float NearZ = 0;			//near Z plane
	float FarZ = 1;				//far Z plane
	double Nobjx, Nobjy, Nobjz; //near points coordinates
	double Fobjx, Fobjy, Fobjz; //far points coordinates
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluUnProject(MouseX, MouseY, NearZ, modelview, projection, viewport, &Nobjx, &Nobjy, &Nobjz);
	gluUnProject(MouseX, MouseY, FarZ, modelview, projection, viewport, &Fobjx, &Fobjy, &Fobjz);

	double rayX = Fobjx - Nobjx;
	double rayY = Fobjy - Nobjy;
	double rayZ = Fobjz - Nobjz;
	double unit = sqrt(rayX * rayX + rayY * rayY + rayZ * rayZ);
	rayX /= unit;
	rayY /= unit;
	rayZ /= unit;

	if (LeftButton == true && RightButton == false)
	{
		double t = ((30) - Nobjx) / rayX;

		double IntersectX, IntersectY, IntersectZ; //calculated intersect points
		IntersectX = 30;
		IntersectY = Nobjy + t * rayY;
		IntersectZ = Nobjz + t * rayZ;

		if (IntersectX > 28 && IntersectX < 32 &&
			IntersectY > 8 && IntersectY < 12 &&
			IntersectZ > -22 && IntersectZ < -18)
		{
			firework(rand() % 10, rand() % 10, 0);
		}
	}
	if (LeftButton == true && RightButton == true){
		double t = ((15) - Nobjy) / rayY;

		double IntersectX, IntersectY, IntersectZ; //calculated intersect points
		IntersectX = Nobjx + t * rayX;
		IntersectY = 15;
		IntersectZ = Nobjz + t * rayZ;

		if (IntersectX > heartx - 2 && IntersectX < heartx + 2 &&
			IntersectY > 14 && IntersectY < 25 &&
			IntersectZ > heartz - 2 && IntersectZ < heartz + 2)
		{
			heartx = IntersectX;
			heartz = IntersectZ;
			if (heartx > 20){
				heartx = 20;
			}
			if (heartx < -20){
				heartx = -20;
			}
			if (heartz > 20){
				heartz = 20;
			}
			if (heartz < -20){
				heartz = -20;
			}
			Pastrybags(heartx,heartz);
			//firework(heartx, heartz, 0);
		}
	}
}
}

void clear()
{
	for (int i = 0; i < Grids.size(); i++)
	{
		Grids[i].mat = 0;
	}
}

void SpecialKey(int key, int x, int y)
{
	
	switch (key)
	{
	
	case GLUT_KEY_UP:
		if (scaley >= 10) {
			scaley = scaley - 2;
		}
		break;
	case GLUT_KEY_LEFT:
		scalex = scalex + 5;
		if (scalex >= 180) {
			scalex -= 360;
		};
		break;
	case GLUT_KEY_DOWN:
		if (scaley <= 150) {
			scaley = scaley + 2;
		}
		break;
	case GLUT_KEY_RIGHT:
		scalex = scalex - 5;
		if (scalex <= -180) {
			scalex += 360;
		}
		break;
	}
	camPos[0] = radius * cos(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0));
	camPos[1] = radius * cos(scaley * (pi / 180.0));
	camPos[2] = radius * sin(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0));

	glutPostRedisplay();
}

/* initial 4 permanent particles */
void initParts(int inM, float inX, float inZ)
{
	float py = 0;
	float x = randf() - 0.5;
	float z = randf() - 0.5;
	float y = 0;
	float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	Particle newpart(inX, py, inZ, x / length, y / length, z / length);
	newpart.is_permanent = true;
	newpart.size = 0.4;
	newpart.speed = 0.2;
	newpart.mat = inM;
	parts.push_back(newpart);
}

//Used to draw obj file
void showObj(string obj)
{
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	int size = list[obj].Fvertex.size();
	for (int i = 0; i < size; i++)
	{
		triple v = list[obj].vNorm[list[obj].Fnorm[i] - 1];
		glNormal3f(v.mX, v.mY, v.mZ);
		pairs t = list[obj].vText[list[obj].Ftext[i] - 1];
		glTexCoord2f(t.mX, t.mY);
		triple m = list[obj].vVertex[list[obj].Fvertex[i] - 1];
		glVertex3f(m.mX, m.mY, m.mZ);
	}
	glEnd();
	glPopMatrix();
}

void initObjects(){
	object obj1(-17, 0, -17, Cone);
	obj1.material = 3;
	objects.push_back(obj1);
	object obj2(17, 0, 17, Cone);
	obj2.material = 2;
	objects.push_back(obj2);
	object obj3(17, 0, -17, Cone);
	obj3.material = 4;
	objects.push_back(obj3);
	object obj4(-17, 0, 17, Cone);
	obj4.material = 1;
	objects.push_back(obj4);

	object obj5(-4, 0, -4, Cone);
	obj5.type = Cube;
	objects.push_back(obj5);
	object obj6(4, 0, 4, Cone);
	obj6.type = Cube;
	objects.push_back(obj6);
	object obj7(4, 0, -4, Cone);
	obj7.type = Cylinder;
	objects.push_back(obj7);
	object obj8(-4, 0, 4, Cone);
	obj8.type = Cylinder;
	objects.push_back(obj8);
	
}

void init()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor4f(1, 1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
	// Initial 4 permanent particles.
	initParts(1, -18.5, 18.5);
	initParts(2, 18.5, 18.5);
	initParts(3, -18.5, -18.5);
	initParts(4, 18.5, -18.5);

	glEnable(GL_TEXTURE_2D);
	img_1 = LoadPPM("ppm/sky1.ppm", &Width1, &Height1, &Max1);
	Welcome_img = LoadPPM("ppm/welcome.ppm", &Width2, &Height2, &Max2);
	CrossSkill_img = LoadPPM("ppm/player.ppm", &Width3, &Height3, &Max3);
	ObjectLoader Obj_1;
	Obj_1 = ObjectLoader();
	Obj_1.loadObject("obj/Saturn.obj");
	list["Obj_1"] = Obj_1;
	ObjectLoader Obj_2;
	Obj_2 = ObjectLoader();
	Obj_2.loadObject("obj/heart.obj");
	list["Obj_2"] = Obj_2;
	
	initObjects();
}

/* draw particles */
void drawparts()
{
	glPushMatrix();
	glTranslatef(0, 1, 0);
	for (int i = 0; i < parts.size(); ++i)
	{
		if (parts[i].age < 0 || !parts[i].is_visible)
			continue;
		int mat = parts[i].mat;
		glPushMatrix();
		glTranslatef(parts[i].position[0], parts[i].position[1], parts[i].position[2]);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[parts[i].mat]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);

		glColor4f(ambMat[mat][0], ambMat[mat][1], ambMat[mat][2], 1);
		glutSolidSphere(parts[i].size, 16, 16);

		glPopMatrix();
	}
	glPopMatrix();
}

void showendtext()
{	
	if (!freemode){
	string String1 = "Player 1 wins!!!";
	string String2 = "Player 2 wins!!!";
	string String3 = "Player 3 wins!!!";
	string String4 = "Player 4 wins!!!";
	string String5 = "Wanna play it again ? Press Y/n.";
	int len = String1.length();
	int len5 = String5.length();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 800);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(200, 100);
	if (heart1 && !heart2 && !heart3 && !heart4){
		for (int i = 0; i < len; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String1[i]);
		}
		for (int i = 0; i < len5; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String5[i]);
		}
		endgame = true;
	}
	if (!heart1 && heart2 && !heart3 && !heart4){
		for (int i = 0; i < len; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String2[i]);
		}
		for (int i = 0; i < len5; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String5[i]);
		}
		endgame = true;
	}
	if (!heart1 && !heart2 && heart3 && !heart4){
		for (int i = 0; i < len; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String3[i]);
		}
		for (int i = 0; i < len5; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String5[i]);
		}
		endgame = true;
	}
	if (!heart1 && !heart2 && !heart3 && heart4){
		for (int i = 0; i < len; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String4[i]);
		}
		for (int i = 0; i < len5; ++i){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String5[i]);
		}
		endgame = true;
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
}

void showtext()
{
	std::string score1 = std::to_string(Player1.points);
	string String1 = "Player 1 Points : " + score1;
	std::string score2 = std::to_string(Player2.points);
	string String2 = "Player 2 Points : " + score2;
	std::string score3 = std::to_string(Player3.points);
	string String3 = "Player 3 Points : " + score3;
	std::string score4 = std::to_string(Player4.points);
	string String4 = "Player 4 Points : " + score4;
	int len = String1.length();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 800);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(0, 770);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String1[i]);
	}
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(0, 750);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String2[i]);
	}
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(0, 730);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String3[i]);
	}
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(0, 710);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String4[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void welcomepage()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 800);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRasterPos2i(250 + Width2, 300);
	glPixelZoom(-1, 1);
	if (!startgame)
	{
		glDrawPixels(Width2, Height2, GL_RGB, GL_UNSIGNED_BYTE, Welcome_img);
	}
}
void CrossSkillpage() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 800);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRasterPos2i(410 + Width3, 600);
	glPixelZoom(-0.7, 0.7);
	
	glDrawPixels(Width3, Height3, GL_RGB, GL_UNSIGNED_BYTE, CrossSkill_img);
	
}
float bar1[3] = {-20, 12, 0};
float bar2[3] = {-20, 10, 0};
float bar3[3] = {-20, 8, 0};
float bar4[3] = {-20, 6, 0};

void drawObjects()
{
	for (int i = 0; i < objects.size(); i++)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[objects[i].material]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[objects[i].material]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[objects[i].material]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);

		glPushMatrix();
		glTranslatef(objects[i].position[0], objects[i].position[1], objects[i].position[2]);
		glRotatef(270, 1, 0, 0);
		switch (objects[i].type)
		{
		case Cylinder:
			glutSolidCylinder(objects[i].size, objects[i].size, 16, 16);
			break;
		case Cube:
			glutSolidCube(2*objects[i].size);
			break;
		case Cone:
			glutSolidCone(objects[i].size, objects[i].size, 16, 16);
			break;
		default:
			cout << "Fail to draw" << endl;
			break;
		}
		glPopMatrix();
	}
}

/* display function  */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	welcomepage();
	CrossSkillpage();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 150);
	showtext();
	showendtext();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, upPos[0], upPos[1], upPos[2]);
	glColor4f(1, 1, 1, 1);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);

	drawgridfloor();
	drawparts();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[1]);
	drawBox(bar1, 2, 1, float(Player1.points) / 30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[2]);
	drawBox(bar2, 2, 1, float(Player2.points) / 30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[3]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[3]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[3]);
	drawBox(bar3, 2, 1, float(Player3.points) / 30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[4]);
	drawBox(bar4, 2, 1, float(Player4.points) / 30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[5]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[5]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[5]);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width1, Height1, 0, GL_RGB, GL_UNSIGNED_BYTE, img_1);

	glPushMatrix();
	float origin[3] = {0, 0, 0};
	glTranslatef(-40, 0, 0);
	drawBox(origin, 0, 80, 80);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 10, -20);
	// glutSolidCube(2);
	glPopMatrix();
	drawObjects();
	if (freemode){
		glPushMatrix();
		glTranslatef(heartx, 15, heartz);
		glRotatef(-90, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.008, 0.008, 0.008);
		showObj("Obj_1");
		glPopMatrix();
	} else {
		if (heart2){
			glPushMatrix();
	glTranslatef(18, 8, 18);
	glRotatef(-90, 1, 0, 0);
	glRotatef(angle, 0, 0, 1);
	glScalef(0.1, 0.1, 0.1);
	showObj("Obj_2");
	glPopMatrix();
		}
		if (heart4){
			glPushMatrix();
	glTranslatef(18, 8, -18);
	glRotatef(-90, 1, 0, 0);
	glRotatef(angle, 0, 0, 1);
	glScalef(0.1, 0.1, 0.1);
	showObj("Obj_2");
	glPopMatrix();
		}
		if (heart3){
			glPushMatrix();
	glTranslatef(-18, 8, -18);
	glRotatef(-90, 1, 0, 0);
	glRotatef(angle, 0, 0, 1);
	glScalef(0.1, 0.1, 0.1);
	showObj("Obj_2");
	glPopMatrix();
			
		}
	if (heart1){
		glPushMatrix();
	glTranslatef(-18, 8, 18);
	glRotatef(-90, 1, 0, 0);
	glRotatef(angle, 0, 0, 1);
	glScalef(0.1, 0.1, 0.1);
	showObj("Obj_2");
	glPopMatrix();
	}
	}

	glutSwapBuffers();
}

void countScore(void)
{
	vector<grid>::iterator i;
	Player1.points = 0;
	Player2.points = 0;
	Player3.points = 0;
	Player4.points = 0;
	for (i = Grids.begin(); i != Grids.end(); ++i)
	{
		if (i->mat == Player1.mat)
		{
			Player1.points++;
		}
		if (i->mat == Player2.mat)
		{
			Player2.points++;
		}
		if (i->mat == Player3.mat)
		{
			Player3.points++;
		}
		if (i->mat == Player4.mat)
		{
			Player4.points++;
		}
	}
}
/* Delete a particle when its age greater than default life duration */
void deleteParticle(int i)
{
	parts[i] = parts[parts.size() - 1];
	parts.pop_back();
}

void getHit(object &obj, Particle &part)
{
	switch (obj.type)
	{
	case Cylinder:
		firework(part.position[0], part.position[2], part.mat);
		break;
	case Cube:
		fountain(part.position[0], 0, part.position[2], part.mat);
		break;
	case Cone:
		if (obj.material != part.mat){
			obj.counter--;
		}
		break;
	default:
		cout << "Fail to call hit func." << endl;
		break;
	}
}

/* move all particles */
void moveAll()
{
	if (startgame)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			float pX, pY, pZ;
			// Negative age to delay particle creation, so particles with negative age won't be moved
			if (parts[i].is_firework)
			{
				parts[i].position[1] += parts[i].direction[1] * parts[i].speed;
				continue;
			}
			if (parts[i].age < 0)
				continue;
			pX = parts[i].position[0] + parts[i].direction[0] * parts[i].speed;
			pY = parts[i].position[1] + parts[i].direction[1] * parts[i].speed;
			pZ = parts[i].position[2] + parts[i].direction[2] * parts[i].speed;
			float x, y, z, length;
			x = parts[i].direction[0] * parts[i].speed + 0.01 * (randf() - 0.5f);
			y = parts[i].direction[1] * parts[i].speed - 0.1;
			z = parts[i].direction[2] * parts[i].speed + 0.01 * (randf() - 0.5f);
			length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			parts[i].direction[0] = x / length;
			parts[i].direction[1] = y / length;
			parts[i].direction[2] = z / length;
			// Change direction if it reaches boundary.
			if (pX > 20 || pX < -20)
			{
				parts[i].direction[0] = -parts[i].direction[0];
			}
			if (pZ > 20 || pZ < -20)
			{
				parts[i].direction[2] = -parts[i].direction[2];
			}

			// Object collision detect
			if (parts[i].is_permanent)
			{
				for (int j = 0; j < objects.size(); j++)
				{
					float x = parts[i].position[0] - objects[j].position[0];
					float y = parts[i].position[1] - objects[j].position[1];
					float z = parts[i].position[2] - objects[j].position[2];
					if (pow(x, 2) + pow(y, 2) + pow(z, 2) < parts[i].size + objects[j].size && objects[j].colddown == 0)
					{
						// Collision is detected
						objects[j].colddown = 60;
						objects[i].getHit(parts[i].mat);
						getHit(objects[j], parts[i]);
						parts[i].direction[2] = -parts[i].direction[2];
						parts[i].direction[0] = -parts[i].direction[0];
					}
				}
			}

			// Change direction if it reaches the floor.
			if (pY < 0 && pY < 0)
			{
				parts[i].direction[1] = -parts[i].direction[1];
				int gridNum = grid::getNumber(pX, pZ);
				if (Grids[gridNum].mat != parts[i].mat)
				{
					if (Grids[gridNum - 1].mat == parts[i].mat || Grids[gridNum + 1].mat == parts[i].mat)
					{
						parts[i].direction[2] = -parts[i].direction[2];
					}
					if (Grids[gridNum - 40].mat == parts[i].mat || Grids[gridNum + 40].mat == parts[i].mat)
					{
						parts[i].direction[0] = -parts[i].direction[0];
					}
					if (Grids[gridNum - 1].mat != parts[i].mat && Grids[gridNum + 1].mat != parts[i].mat && Grids[gridNum - 40].mat != parts[i].mat && Grids[gridNum + 40].mat != parts[i].mat)
					{
						parts[i].direction[2] = -parts[i].direction[2];
						parts[i].direction[0] = -parts[i].direction[0];
					}
					Grids[gridNum].mat = parts[i].mat;
				}

				// Decrease its speed unless it is permanent paritcle.
				if (!parts[i].is_permanent)
				{
					if (parts[i].speed > 0.05)
						parts[i].speed = 0.6 * (parts[i].speed - 0.05) + 0.03;
					else
						parts[i].speed = 0;
				}
			}
			parts[i].position[0] += parts[i].direction[0] * parts[i].speed;
			parts[i].position[1] += parts[i].direction[1] * parts[i].speed;
			parts[i].position[2] += parts[i].direction[2] * parts[i].speed;
		}
	}
}

/* Check if mouse ray intesect down left 3 planes */
void intersectDownLeft(object &object, int plane)
{
	float planeConst, d;
	planeConst = object.DownLeft[plane];
	if (Rd[plane] == 0)
		return;
	d = (planeConst - m_start[plane]) / Rd[plane];
	if (d < 0)
		return;
	if (m_start[(plane + 4) % 3] + d * Rd[(plane + 4) % 3] < object.DownLeft[(plane + 4) % 3] || m_start[(plane + 4) % 3] + d * Rd[(plane + 4) % 3] > object.UpperRight[(plane + 4) % 3])
		return;
	if (m_start[(plane + 5) % 3] + d * Rd[(plane + 5) % 3] < object.DownLeft[(plane + 5) % 3] || m_start[(plane + 5) % 3] + d * Rd[(plane + 5) % 3] > object.UpperRight[(plane + 5) % 3])
		return;
	if (object.distToMouseRay < 0 || d < object.distToMouseRay)
	{
		object.distToMouseRay = d;
		return;
	}
}

/* Check if mouse ray intesect upper right 3 planes */
void intersectUpperRight(object &object, int plane)
{
	float planeConst, d;
	planeConst = object.UpperRight[plane];
	if (Rd[plane] == 0)
		return;
	d = (planeConst - m_start[plane]) / Rd[plane];
	if (d < 0)
		return;
	if (m_start[(plane + 4) % 3] + d * Rd[(plane + 4) % 3] < object.DownLeft[(plane + 4) % 3] || m_start[(plane + 4) % 3] + d * Rd[(plane + 4) % 3] > object.UpperRight[(plane + 4) % 3])
		return;
	if (m_start[(plane + 5) % 3] + d * Rd[(plane + 5) % 3] < object.DownLeft[(plane + 5) % 3] || m_start[(plane + 5) % 3] + d * Rd[(plane + 5) % 3] > object.UpperRight[(plane + 5) % 3])
		return;
	if (object.distToMouseRay < 0 || d < object.distToMouseRay)
	{
		object.distToMouseRay = d;
		return;
	}
}

void boxRayPick(object &obj)
{
	intersectDownLeft(obj, 0);
	intersectDownLeft(obj, 1);
	intersectDownLeft(obj, 2);
	intersectUpperRight(obj, 0);
	intersectUpperRight(obj, 1);
	intersectUpperRight(obj, 2);
}

void rayPick()
{
	// Set disToMouseRay for exist objects
	for (int i = 0; i < objects.size(); i++)
	{
		boxRayPick(objects[i]);
	}

	// Find the closest object to select
	int idx = -1;
	float distance = 10000;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].distToMouseRay > 0 && objects[i].distToMouseRay < distance)
		{
			idx = i;
			distance = objects[i].distToMouseRay;
		}
	}

	picked_object = idx;
}
void hideheart(int i){
	switch (i){
		case 0:
			heart1 = false;
			break;
		case 1:
			heart2 = false;
			break;
		case 2:
			heart3 = false;
			break;
		case 3:
			heart4 = false;
			break;
	}
}
void resetgame(){
	clear();
	parts.clear();
	initParts(1, -18.5, 18.5);
	initParts(2, 18.5, 18.5);
	initParts(3, -18.5, -18.5);
	initParts(4, 18.5, -18.5);
	objects.clear();
	initObjects();
	heart1 = true;
	heart2 = true;
	heart3 = true;
	heart4 = true;
}
void spin(int val)
{
	angle +=1;
	glutPostRedisplay();
	glutTimerFunc(17, spin, 0);
}

/* FPS function - 60 FPS per second */
void FPS(int val)
{
	countScore();
	for (int i = 1; i <= gameSpeed; i++)
	{
		moveAll();
	}
	glutPostRedisplay();
	for (int i = 0; i < parts.size(); i++)
	{
		if (!parts[i].is_permanent)
			parts[i].age += gameSpeed;
		if (parts[i].age >= DEFAULT_LIFE_DURATION)
		{
			if (parts[i].is_firework)
			{
				firework1(parts[i].position[0], parts[i].position[1], parts[i].position[2], parts[i].mat);
			}
			deleteParticle(i);
		}
	}
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].colddown > 0)
		{
			objects[i].colddown--;
		}

		// In case health = 0, destruct object and set parts to invisible
		if (objects[i].counter == 0 && !freemode)
		{
			for (int j = 0; j < parts.size(); j++)
			{
				if (parts[j].mat == objects[i].material) {
					parts[j].speed = 0;
					parts[j].is_visible = false;
					hideheart(j);
				}
			}
			objects[i] = objects[objects.size()-1];
			objects.pop_back();
		}
	}
	glutTimerFunc(17, FPS, 0);
}
void Mouse(int btn, int state, int x, int y)
{
	if (state == GLUT_UP)
	{
		LeftButton = false;
		RightButton = false;
		middleButton = false;
	}
	
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		LeftButton = true;
		if (startgame)
		{
			mouseHandler.leftClickDown(x, 800-y);
		}
	}
	
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		RightButton = true;
	}
	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		middleButton = true;
	}
	MouseX = x;
	MouseY = 800 - y;

	if (!startgame)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			startHandler.leftClickDown(x, 800 - y); \
		}
	}
	else
	{
		Intersectiontest();
	}

	double matModelView[16], matProjection[16];
	int viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double winX = (double)x;
	double winY = viewport[3] - (double)y;

	gluUnProject(winX, winY, 0.0, matModelView, matProjection,
				 viewport, &m_start[0], &m_start[1], &m_start[2]);

	gluUnProject(winX, winY, 1.0, matModelView, matProjection,
				 viewport, &m_end[0], &m_end[1], &m_end[2]);

	// Set Rd vector
	double xDiff = m_end[0] - m_start[0];
	double yDiff = m_end[1] - m_start[1];
	double zDiff = m_end[2] - m_start[2];

	double mag = sqrt(pow(xDiff, 2) + pow(yDiff, 2) + pow(zDiff, 2));

	Rd[0] = xDiff / mag;
	Rd[1] = yDiff / mag;
	Rd[2] = zDiff / mag;

	// New Raypick
	if (state == GLUT_DOWN)
	{
		// Clear previous select data
		picked_object = -1;
		for (int i = 0; i < objects.size(); i++)
		{
			objects[i].distToMouseRay = -1;
		}

		rayPick();
	}

	// Put down objects
	if (state == GLUT_UP && picked_object != -1){
		if (Rd[1] == 0) return;
		float d = (0 - m_start[1])/Rd[1];
		float x = m_start[0] + Rd[0] * d;
		float z = m_start[2] + Rd[2] * d;
		if (x > -20 && x < 20 && z > -20 && z < 20){
			objects[picked_object].move(x-objects[picked_object].position[0], 0, z-objects[picked_object].position[2]);
		}
	}
}
void keyboard(unsigned char key, int x, int y)
{
	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
	case 'q':
	case 27:
		exit(0);
		break;
	case 'a':
		if (startgame)
		{
			firework(rand() % 10, rand() % 10, 0);
		}
		break;
	case 'r':
		clear();
		break;
	case 'o':
		objects.push_back(object(0, 0, 0, rand()%3));
		break;
	case 'f':
	case 'F':
		if (startgame)
		{
			fountain((randf() - 0.5) * 15, 0, (randf() - 0.5) * 15);
		}
		break;
	case 'R':
		while (int i = parts.size() > 4)
		{
			parts.pop_back();
		}
		break;
	case 'c':
	case 'C':
		freemode = !freemode;
		break;
	case 'Y':
		if (endgame){
			endgame = false;
			startgame = false;
			resetgame();
		}
		break;
	case 'n':
		if (endgame){
			exit(0);
		}
		break;
	case '+':
		gameSpeed += 1;
		break;
	case '-':
		if (gameSpeed > 1)
			gameSpeed -= 1;
		break;
	}
	glutPostRedisplay();
}
void motion(int x,int y){
	if (LeftButton == true && RightButton == true){
		MouseX = x;
		MouseY = 800 - y;
		Intersectiontest();
	}
	glutPostRedisplay();
}
void wheel(int wheel, int dir, int x, int y) {
	if (dir > 0) {
		radius = max(10.0f, radius - 3);
	}
	else if (dir < 0) {
		radius = min(100.0f, radius + 3);
	}
	camPos[0] = radius * cos(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0));
	camPos[1] = radius * cos(scaley * (pi / 180.0));
	camPos[2] = radius * sin(scalex * (pi / 180.0)) * sin(scaley * (pi / 180.0));

	glutPostRedisplay();
}
void STARTfree()
{
	startgame = true;
	freemode = true;
	std::cout << "GAME STARTS!!! Feel free to enjoy!!" << endl;
}
void STARTmatch()
{
	startgame = true;
	freemode = false;
	std::cout << "GAME STARTS!!! Win the game!!" << endl;
}
void Skill(int player) {
	vector<grid>::iterator i;
	vector<Particle>::iterator j;
	for (i = Grids.begin(); i != Grids.end(); ++i)
	{
		if (freemode) {
			if (i->mx == Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mx) {
				i->mat = parts[player].mat;
			}
			if (i->mz == Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mz) {
				i->mat = parts[player].mat;
			}
		}
		if (!freemode) {
			if (i->mx <= Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mx + 2 && i->mx >= Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mx - 2) {
				if (i->mz <= Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mz + 2 && i->mz >= Grids[grid::getNumber(parts[player].position[0], parts[player].position[2])].mz - 2) {
					i->mat = parts[player].mat;
				}
			}
		}
	}
}
void CrossSkill1() {
	std::cout << "Player1 skill" << endl;

	Skill(0);
}
void CrossSkill2() {
	std::cout << "Player2 skill" << endl;
	Skill(1);
}
void CrossSkill3() {
	std::cout << "Player3 skill" << endl;
	Skill(2);
}
void CrossSkill4() {
	std::cout << "Player4 skill" << endl;
	Skill(3);
}
Handler freemode1 = {
	300,
	430,
	400,
	380,
	STARTfree};

Handler matchmode = {
	485,
	630,
	400,
	380,
	STARTmatch};
Handler Cross1 = {
	670,720,800-12,800-38,CrossSkill1
};
Handler Cross2 = {
	670,720,800-50,800-80,CrossSkill2
};
Handler Cross3 = {
	670,720,800-90,800-120,CrossSkill3
};
Handler Cross4 = {
	670,720,800-130,800-160,CrossSkill4
};
/* main function */
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	startHandler.addHandler(&freemode1);
	startHandler.addHandler(&matchmode);
	mouseHandler.addHandler(&Cross1);
	mouseHandler.addHandler(&Cross2);
	mouseHandler.addHandler(&Cross3);
	mouseHandler.addHandler(&Cross4);
	glutCreateWindow("marble war");

	GenFloor();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKey);
	glutMouseFunc(Mouse);
	glutTimerFunc(17, FPS, 0);
	glutTimerFunc(17,spin,0);
	glutMouseWheelFunc(wheel);
	glutMotionFunc(motion);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_DST_COLOR);

	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	init();

	glutMainLoop();

	return (0);
}
