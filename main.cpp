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


std::random_device rd; 
std::mt19937 gen(rd()); 
std::uniform_real_distribution<float> dis(0.0, 0.5);

float camPos[] = {0, 60, 42}; 
float upPos[] = {0, 1, 0};

GLubyte* img_1;
int Width1, Height1, Max1;
GLubyte* Welcome_img;
int Width2, Height2, Max2;
unsigned int Texture[3];

int gameSpeed = 1;

// TOBE FIXED 2. two blue balls have similar color, have best to change one of it.
float ambMat[7][4] = {{0, 0, 0, 1}, {0.2, 0.6, 0.2, 1}, {0.6, 0.2, 0.2, 1}, {0.2, 0.2, 0.6, 1},{ 0.2295f, 0.08825f, 0.0275f, 1.0f },{ 0.5, 0.4, 0.3, 0.2 },{ 0.05f,0.05f,0.0f,1.0f }};
float diffMat[7][4] = {{0.5, 0, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 0, 1, 0},{0.5508f, 0.2118f, 0.066f, 1.0f },{0.396f, 0.74151f, 0.69102f, 0.8f },{0.5f,0.5f,0.4f,1.0f}};
float specMat[7][4] = {{0, 0.5, 0, 1}, {0, 0.5, 0.5, 1}, {0, 1, 0, 1}, {1, 1, 1, 0},{0.580594f, 0.223257f, 0.0695701f, 1.0f },{0.297254f, 0.30829f, 0.306678f, 0.8f},{0.7f,0.7f,0.04f,1.0f}};

float scaley = 0;
float scalex = 0;
bool blend = false;
int MouseX = 0;			//Mouse position X
int MouseY = 0;			//Mouse position Y
bool LeftButton = false; // track left mouse status
bool RightButton = false; // track right mouse status
bool middleButton = false; //track middle mouse status
bool startgame = false;
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
struct Handler {
    unsigned int mLeft, mRight, mTop, mBottom;
    void (*mHandlerFunc)();

    bool isInBounds(unsigned int x, unsigned int y) {
        /**
         * YOUR CODE HERE, replace `false` with your expression.
         */
        if (x <= mRight && x >= mLeft && y <= mTop && y >= mBottom){
            return true;
        } else {
            return false;
        }
    }

    void handleClickAt(unsigned int x, unsigned int y) {
        if (isInBounds(x, y)) {
            mHandlerFunc();
        }
    }

    void drawBoxVertices() {
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

struct InteractionHandler {
    std::vector<Handler *> mHandlers;

    void leftClickDown(int x, int y) {
        //std::cout << "Left click at " << x << ", " << y << std::endl;
        for (Handler *handler : mHandlers) {
            handler->handleClickAt(x,y);
        }
    }

    void drawHandlers() {
        glColor3f(1, 1, 1);
        glLineWidth(2);
        glBegin(GL_LINES);
        for (Handler *handler : mHandlers) {
            handler->drawBoxVertices();
        }
        glEnd();
    }

    void addHandler(Handler *handler) {
        mHandlers.push_back(handler);
    }
};

InteractionHandler mouseHandler;

GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    //printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    nm = n*m;
    
    img = (GLubyte*)(malloc(3*sizeof(GLuint)*nm));
    
    s=255.0/k;
    
    
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    *width = n;
    *height = m;
    *max = k;
    
    return img;
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
		newpart.speed = 0.5;
		newpart.age = -3*i;
		parts.push_back(newpart);
	}
}

/* drawPolygon - takes 4 indices and an array of vertices
 *   and draws a polygon using the vertices indexed by the indices
 */
void drawPolygon(int a, int b, int c, int d, float v[8][3]){
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
    	// int mat = rand()%7;
		newGrid = grid(i,0,H,mx,mz);
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
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[Grids[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[Grids[i].mat]);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);
		float origin[3] = {Grids[i].mx,0,Grids[i].mz};
		drawBox(origin,1,Grids[i].Height,1);
	}	
	//glutPostRedisplay();
}

void Intersectiontest(){
	double projection[16];
	double modelview[16];
	int viewport[4];
	float NearZ = 0;	//near Z plane
	float FarZ = 1;		//far Z plane
  	double Nobjx,Nobjy,Nobjz;	//near points coordinates
	double Fobjx,Fobjy,Fobjz;	//far points coordinates
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetIntegerv( GL_VIEWPORT, viewport );	
	gluUnProject( MouseX, MouseY, NearZ , modelview, projection, viewport, &Nobjx, &Nobjy, &Nobjz );
	gluUnProject( MouseX, MouseY, FarZ , modelview, projection, viewport, &Fobjx, &Fobjy, &Fobjz );

	double rayX = Fobjx - Nobjx;
	double rayY = Fobjy - Nobjy;
	double rayZ = Fobjz - Nobjz;
	double unit = sqrt(rayX*rayX + rayY*rayY + rayZ*rayZ);
	rayX /= unit;
	rayY /= unit;
	rayZ /= unit;
	
	if (LeftButton == true){
	double t = ((30) - Nobjx)/rayX;

	double IntersectX,IntersectY,IntersectZ;	//calculated intersect points
	IntersectX = 30;
	IntersectY = Nobjy + t * rayY;
	IntersectZ = Nobjz + t * rayZ;
	
		if(IntersectX > 28 && IntersectX < 32 &&
		IntersectY > 8 && IntersectY < 12 &&
		IntersectZ > -22 && IntersectZ < -18){
			firework(rand() % 10, rand() % 10);
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
			exit (0);
			break;
		case 'a':
		if (startgame){
			firework(rand() % 10, rand() % 10);
		}
		break;
	case 'p':
		partcam = -1;
		break;
	case 'f':
	case 'F':
		if (startgame){
			fountain((randf()-0.5)*15,0,(randf()-0.5)*15);
		}
		break;

	case 'r':
	case 'R':
		while (int i = parts.size() > 4)
		{
			parts.pop_back();
		}
		break;
	case 'b':
	case 'B':
		blend = !blend;
		if (blend){
		glEnable(GL_BLEND);
	}	else {
		glDisable(GL_BLEND);
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

/* initial 4 permanent particles */
void initParts(int inM, float inX, float inZ){
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

//GLubyte wb[2] = { 0x00, 0xff };
//GLubyte checker[512];
void init(void)
{	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor4f(1, 1, 1,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
	// Initial 6 permanent particles.
	initParts(1,-18.5,18.5);
	initParts(2,18.5,18.5);
	initParts(3,-18.5,-18.5);
	initParts(4,18.5,-18.5);
	/*int i, j; 
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 8; j++) {
			checker[i * 8 + j] = wb[(i / 8 + j) % 2];
		}
	} */

	glEnable(GL_TEXTURE_2D);
	img_1 = LoadPPM("ppm/sky1.ppm",&Width1, &Height1, &Max1);
	Welcome_img = LoadPPM("ppm/welcome1.ppm",&Width2, &Height2, &Max2);
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

		glColor4f(ambMat[mat][0], ambMat[mat][1], ambMat[mat][2],1);
		glutSolidSphere(parts[i].size, 16, 16);

		glPopMatrix();
	}
	glPopMatrix();
}

void showtext(){
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
	for (int i = 0; i < len; ++i) {
    	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String1[i]);
	}
    glPopMatrix();
	glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(0, 750);
	for (int i = 0; i < len; ++i) {
    	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String2[i]);
	}
    glPopMatrix();
	glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(0, 730);
	for (int i = 0; i < len; ++i) {
    	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String3[i]);
	}
    glPopMatrix();
	glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(0, 710);
	for (int i = 0; i < len; ++i) {
    	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String4[i]);
	}
    glPopMatrix();
	glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
void welcomepage(){
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glRasterPos2i(250 + Width2, 300);
    glPixelZoom(-1, 1);
	if (!startgame){
		glDrawPixels(Width2,Height2,GL_RGB,GL_UNSIGNED_BYTE,Welcome_img);
	}
    
}
float bar1[3] = { -20,12,0 };
float bar2[3] = { -20,10,0 };
float bar3[3] = { -20,8,0 };
float bar4[3] = { -20,6,0 };
/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	welcomepage();
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
	showtext();
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
	glColor4f(1, 1, 1,1);
	
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
	drawgridfloor();
	drawparts();
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[1]);
	drawBox(bar1, 2, 1, float(Player1.points)/30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[2]);
	drawBox(bar2, 2, 1, float(Player2.points) /30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[3]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[3]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[3]);
	drawBox(bar3, 2, 1, float(Player3.points) /30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[4]);
	drawBox(bar4, 2, 1, float(Player4.points) /30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[5]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[5]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[5]);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width1, Height1, 0, GL_RGB, GL_UNSIGNED_BYTE, img_1);

	glPushMatrix();
	float origin[3] = {0,0,0};
	glTranslatef(-40,0,0);
	drawBox(origin, 0,80,80);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30,10,-20);
	glutSolidCube(2);
	glPopMatrix();
	
	//glPixelZoom(-0.5f, -0.5f);
	//glRasterPos2i(1, 1);
	//glCopyPixels(10, -10,20,20 GL_RGB);
	//cout << Player1.points << endl;
	glutSwapBuffers();
	
}

void countScore(void) {
	vector<grid>::iterator i;
	Player1.points = 0;
	Player2.points = 0;
	Player3.points = 0;
	Player4.points = 0;
	for (i = Grids.begin(); i != Grids.end(); ++i) {
		if (i->mat == Player1.mat) {
			Player1.points++;
		}
		if (i->mat == Player2.mat) {
			Player2.points++;
		}
		if (i->mat == Player3.mat) {
			Player3.points++;
		}
		if (i->mat == Player4.mat) {
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

/* move all particles */
void moveAll()
{	if (startgame){
	for (int i = 0; i < parts.size(); i++)
	{
		float pX, pY, pZ;
		// Negative age to delay particle creation, so particles with negative age won't be moved
		if (parts[i].age < 0) continue;
		pX = parts[i].position[0] + parts[i].direction[0] * parts[i].speed;
		pY = parts[i].position[1] + parts[i].direction[1] * parts[i].speed;
		pZ = parts[i].position[2] + parts[i].direction[2] * parts[i].speed;
		float x, y, z, length;
		x = parts[i].direction[0] * parts[i].speed;
		y = parts[i].direction[1] * parts[i].speed - 0.1;
		z = parts[i].direction[2] * parts[i].speed + 0.01*(randf()-0.5f);
		length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		parts[i].direction[0] = x / length;
		parts[i].direction[1] = y / length;
		parts[i].direction[2] = z / length;
		// Change direction if it reaches boundary.
		if (pX > 20 || pX < -20){
			parts[i].direction[0] = -parts[i].direction[0];
		}
		if (pZ > 20 || pZ < -20){
			parts[i].direction[2] = -parts[i].direction[2];
		}
		// Change direction if it reaches the floor.
		if (pY < 0 && pY < 0)
		{
			parts[i].direction[1] = -parts[i].direction[1];
			int gridNum = grid::getNumber(pX,pZ);
			if (Grids[gridNum].mat != parts[i].mat){
				if(Grids[gridNum-1].mat == parts[i].mat ||Grids[gridNum+1].mat == parts[i].mat){
					parts[i].direction[2] = -parts[i].direction[2];
				}
				if(Grids[gridNum-40].mat == parts[i].mat ||Grids[gridNum+40].mat == parts[i].mat){
					parts[i].direction[0] = -parts[i].direction[0];
				}
				if(Grids[gridNum-1].mat != parts[i].mat &&Grids[gridNum+1].mat != parts[i].mat
					&&Grids[gridNum-40].mat != parts[i].mat &&Grids[gridNum+40].mat != parts[i].mat){
						parts[i].direction[2] = -parts[i].direction[2];
						parts[i].direction[0] = -parts[i].direction[0];
					}
				Grids[gridNum].mat = parts[i].mat;
			}

			// Decrease its speed unless it is permanent paritcle.
			if (!parts[i].is_permanent)
				parts[i].speed *= 0.7;
		}
		parts[i].position[0] += parts[i].direction[0] * parts[i].speed;
		parts[i].position[1] += parts[i].direction[1] * parts[i].speed;
		parts[i].position[2] += parts[i].direction[2] * parts[i].speed;
	}
	}
}


/* FPS function - 60 FPS per second */
void FPS(int val)
{
	countScore();
	for (int i = 1; i <= gameSpeed; i++){
		moveAll();
	}
	glutPostRedisplay();
	for (int i = 0; i < parts.size(); i++)
	{
		if (!parts[i].is_permanent)
			parts[i].age+=gameSpeed;
		if (parts[i].age >= DEFAULT_LIFE_DURATION)
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
void Mouse(int btn, int state, int x, int y){
	if (state == GLUT_UP){
        LeftButton = false;
        RightButton = false;
		middleButton = false;
    }
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        LeftButton = true;
    }
    if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        RightButton = true;
    }
	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
        middleButton = true;
    }
	MouseX = x;
	MouseY = 800 - y;

	if (!startgame){
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
			mouseHandler.leftClickDown(x,800 - y);
		}
	}	else {
		Intersectiontest();
	}
	//std::cout<<MouseX<<" "<<MouseY<<endl; 
	glutPostRedisplay();
}
void START(){
	startgame = true;
	std::cout<<"GAME STARTS!!!"<<endl;
}

Handler Here = {
    400,
    460,
    400,
	380,
    START
};


/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	mouseHandler.addHandler(&Here);
	glutCreateWindow("marble war");	//creates the window
	
	GenFloor();
	glutDisplayFunc(display);	//registers "display" as the display callback function
	//std::cout<<Grids[800].getNumber()<<endl;	
	glutKeyboardFunc(keyboard);
    glutSpecialFunc(SpecialKey);
	glutMouseFunc(Mouse);	
	glutTimerFunc(17, FPS, 0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_DST_COLOR);	

	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
    
	init();
	
	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}
