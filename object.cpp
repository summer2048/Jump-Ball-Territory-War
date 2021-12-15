#include "object.h"
#include <math.h>

object::object(float pX, float pY, float pZ, int type)
	: position {pX,pY,pZ}
	, material(7)
	, type(type)
	, size(1)
	, DownLeft {0,0,0}
	, UpperRight {0,0,0}
	, distToMouseRay(-1)
    , counter(3)
    , colddown(0)
{
	initCorner();
}

void object::initCorner(){
	switch (type){
        case Cylinder:
		case Cone:
            DownLeft[0] = position[0] - size;
			DownLeft[1] = position[1];
			DownLeft[2] = position[2] - size;
			UpperRight[0] = position[0] + size;
			UpperRight[1] = position[1] + size;
			UpperRight[2] = position[2] + size;
			break;
		case Cube:
			DownLeft[0] = position[0] - size;
			DownLeft[1] = position[1] - size;
			DownLeft[2] = position[2] - size;
			UpperRight[0] = position[0] + size;
			UpperRight[1] = position[1] + size;
			UpperRight[2] = position[2] + size;
			break;
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
    colddown = 60;
    /* Add reaction here */
}
