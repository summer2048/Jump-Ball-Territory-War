#include "mathLib2D.h"
#include <cmath>
#include <iostream>
#include <random>

/*I extended the previous 2D points and vectors into 3D with some new classes*/
Point3D::Point3D(){
	this->mX = 0;
	this->mY = 0;
	this->mZ = 0;
}

Point3D::Point3D(float inX, float inY, float inZ){
    mX = inX;
    mY = inY;
	mZ = inZ;
}

float Point3D::distanceTo(Point3D other){   //Distance function
    return sqrt(pow(other.mX - mX,2) + pow(other.mY - mY,2) + pow(other.mZ - mZ,2));
};

float Point3D::fastDistanceTo(Point3D other){   //Fast distance function
    return  pow(other.mX - mX,2) + pow(other.mY - mY,2) + pow(other.mZ - mZ,2);
};

Vec3D::Vec3D(){
	this->mX = 0;
	this->mY = 0;
	this->mZ = 0;
}

Vec3D::Vec3D(float inX, float inY, float inZ){
    mX = inX;
    mY = inY;
	mZ = inZ;
}

float Vec3D::length(){  //Length function
    return  sqrt(pow(mX,2)+pow(mY,2)+pow(mZ,2));
};

Vec3D Vec3D::normalize(){   //Normalize function
    return Vec3D(mX/Vec3D::length(),mY/Vec3D::length(),mZ/Vec3D::length());
};

Vec3D Vec3D::multiply(float scalar){    //Multiply function
    return Vec3D(mX * scalar,mY * scalar,mZ * scalar);
};

Vec3D Vec3D::createVector(Point3D p1, Point3D p2){  //create Vectore function
    return Vec3D(p2.mX-p1.mX,p2.mY-p1.mY,p2.mZ-p1.mZ);
};

Point3D Vec3D::movePoint(Point3D source){   //movePoint function
    return Point3D(source.mX+mX,source.mY+mY,source.mZ+mZ);
};

Angle::Angle(){
	this->rx = 0.0;
	this->ry = 0.0;
	this->rz = 0.0;
}

Angle::Angle(float inX, float inY, float inZ){
	this->rx = inX;
	this->ry = inY;
	this->rz = inZ;
}

Particle::Particle(){
	this->Position= Point3D(0.0,0.0,0.0);
    this->Color= colour(0,0,0);
	this->Size= 0;
	this->Direction= Vec3D(0,0,0);
	this->Speed=0.0;
	this->RAngle=Angle(0,0,0);
	this->Age=0;
	this->Explode=0;
	this->Sproperty=0;	
}

Particle::Particle(Point3D Pos, colour Col, float inS, Vec3D inV, float inSpd, Angle inA, int inAge, bool inE, int inMt, int inSp){
	this->Position=Pos;
	this->Color=Col;
	this->Size=inS;
	this->Direction=inV;
	this->Speed=inSpd;
	this->RAngle=inA;
	this->Age=inAge;
	this->Explode=inE;
	this->Mtype=inMt;
	this->Sproperty=inSp;
}