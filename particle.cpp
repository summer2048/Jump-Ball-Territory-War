#include "particle.h"
#include <math.h>

Particle::Particle()
{
	position[0] = 0;
	position[1] = 0;
	position[2] = 0;
	color[0] = 0;
	color[1] = 1;
	color[2] = 1;
	speed = 0;
	size = 0.1;
	age = 0;
	mat = 0;
	is_permanent = false;
}

Particle::Particle(float pX, float pY, float pZ, float dX, float dY, float dZ)
{
	color[0] = 0;
	color[1] = 1;
	color[2] = 1;
	speed = 0;
	size = 0.1;
	age = 0;
	mat = rand() % 4 + 1;
	position[0] = pX;
	position[1] = pY;
	position[2] = pZ;
	direction[0] = dX;
	direction[1] = dY;
	direction[2] = dZ;
	speed = 0.6;
	is_permanent = false;
}