#ifndef OBJECT_H
#define OBJECT_H

#define Cylinder 0
#define Cube 1

class object
{
public:
	float position[3];
	int material;
	int type;
	float size;
    float DownLeft[3]; // Minimal x,y,z of surronding box
    float UpperRight[3]; // Maximal x,y,z of surronding box
    float distToMouseRay; // Set it to -1 if no intersection
    int counter;
    int colddown;

	object(float pX, float pY, float pZ, int type);
    void resetCorner();
    void getHit(int mat);
    void move(float x, float y, float z);
private:
    void initCorner();
};

#endif