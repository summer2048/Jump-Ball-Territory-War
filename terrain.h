#ifndef TERRAIN_H
#define TERRAIN_H
#include <tuple>
using namespace std;

typedef tuple <int,int,int> colour;

class grid {
public:
    grid();
    grid(colour Col,float inH, float mx, float mz);
    float Height;
    colour color;
    float mx;
    float mz;

    
};

#endif