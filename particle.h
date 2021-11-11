#ifndef HEADER_H
#define HEADER_H

using namespace std;

class Particle
{
public:
	static const int DEFAULT_LIFE_DURATION = 180;
	bool is_permanent;
	float position[3];
	float direction[3];
	float color[3];
	float speed;
	float size;
	int age;
	int mat;
	Particle();
	Particle(float pX, float pY, float pZ, float dX, float dY, float dZ);
};

#endif