
#ifndef MATHLIB_2D_H
#define MATHLIB_2D_H

#include<iostream> 
#include<tuple>
using namespace std; 

typedef tuple <int,int,int> colour; //define the color properties as tuple

/*I extended the previous 2D points and vectors into 3D with some new classes*/
class Point3D {
public:
    Point3D();
    Point3D(float inX, float inY, float inZ);
    float mX;
    float mY;
    float mZ;

    float distanceTo(Point3D other);
    float fastDistanceTo(Point3D other);
};

class Vec3D {
public:
    Vec3D();
    Vec3D(float inX, float inY, float inZ);
    float mX;
    float mY;
    float mZ;
    float length();
    Vec3D normalize();
    Vec3D multiply(float scalar);
    Point3D movePoint(Point3D source);

    static Vec3D createVector(Point3D p1, Point3D p2);
};

class Angle{
public:
    Angle();
    Angle(float inX, float inY, float inZ);
    float rx;
    float ry;
    float rz;
};


class Particle {
public:
    Particle();
    Particle(Point3D Pos, colour Col, float inS, Vec3D inV, float inSpd, Angle inA, int inAge, bool inEx, int inMt, int inSp);
    Point3D Position;
    colour Color;
    float Size;
    Vec3D Direction;
    float Speed;
    Angle RAngle;   //rotation angle
    int Age;    //Age of particle
    bool Explode;   //exploding state variable
    int Mtype;  //material type
    int Sproperty;  //special property type
};


#endif
