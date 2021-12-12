#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H
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
#include <vector>
using namespace std;

struct triple {
public:
    float mX,mY,mZ;
};

struct pairs {
public:
    float mX,mY;
};


class ObjectLoader {
public:
	ObjectLoader(); 
	ObjectLoader(float inX, float inY, float inZ);
	float px,py,pz;
	void loadObject(const char* filename);
	
	vector <triple> vVertex; 
	vector <pairs> vText; 
	vector <triple> vNorm; 
	vector <unsigned int> Fvertex, Ftext, Fnorm;
	
};
#endif