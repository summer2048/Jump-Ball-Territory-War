
#ifndef MARBLE_H
#define MARBLE_H

#include<iostream> 
#include<tuple>
using namespace std; 

typedef tuple <int,int,int> colour; //define the color properties as tuple


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



class Particle {
public:
    Particle();
    Particle(Point3D Pos, colour Col, float inS, Vec3D inV, float inSpd, int inAge);
    Point3D Position;
    colour Color;
    float Size;
    Vec3D Direction;
    float Speed;
    int Age;    //Age of particle
};


#endif
