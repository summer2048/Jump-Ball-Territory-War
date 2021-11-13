#ifndef TERRAIN_H
#define TERRAIN_H
#include <tuple>
using namespace std;

class grid {//TOBE FIXED 3. Add a function getHeight(float x, float z), return height of grid with given x,z value.
public:
    grid();
    grid(int inNum, int mat,float inH, float mx, float mz);
    int getNumber(float x, float z);
    float Height;
    int number;
    int mat;
    float mx;
    float mz;
};

#endif