#include "object.h"
#include <math.h>

object::object(float pX, float pY, float pZ, int type)
	: position {pX,pY,pZ}
	, material(1)
	, type(type)
	, size(1)
	, DownLeft {0,0,0}
	, UpperRight {0,0,0}
	, distToMouseRay(-1)
    , counter(0)
{
	initCorner();
}

void object::initCorner(){
	switch (type){
		default:
			DownLeft[0] = position[0] - size/2;
			DownLeft[1] = position[1] - size/2;
			DownLeft[2] = position[2] - size/2;
			UpperRight[0] = position[0] + size/2;
			UpperRight[1] = position[1] + size/2;
			UpperRight[2] = position[2] + size/2;
			break;
	}
}

void object::move(float x, float y, float z){
    position[0] += x;
    position[1] += y;
    position[2] += z;
    resetCorner();
}

void object::resetCorner(){
    // Lazy approach
	initCorner();
}

void object::getHit(int mat){
    /* Add reaction here */
}
