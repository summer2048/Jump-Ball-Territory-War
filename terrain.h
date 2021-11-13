#ifndef TERRAIN_H
#define TERRAIN_H
#include <tuple>
using namespace std;

class grid {
public:
    grid();
    grid(int inNum, int mat,float inH, float mx, float mz);
    static int getNumber(float x, float z);
    float Height;
    int number;
    int mat;
    float mx;
    float mz;
};

#endif