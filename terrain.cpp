#include "terrain.h"


grid::grid(){
    this->Height = 0;
    this->mat = 0;
    this->mx = 0;
    this->mz = 0;
}

grid::grid(int inNum, int mat,float inH, float inmx, float inmz){
    this->number = inNum;
    this->mat = mat;
    this->Height = inH;
    this->mx = inmx;
    this->mz = inmz;
}

// TOBE FIXED 3. was not 100% correct in edge cases
int grid::getNumber(float x, float z){  //return the current number of grid using x and z
    return (int (x + 19.5) * 40 + int (19.5 - z));
}