#ifndef HEADER_H
#define HEADER_H

#define DEFAULT_LIFE_DURATION 180

using namespace std;

class Particle
{
public:
	bool is_permanent;
	float position[3];
	float direction[3];
	float speed;
	float size;
	int age;
	int mat;
	Particle(float pX, float pY, float pZ, float dX, float dY, float dZ);
};

#endif