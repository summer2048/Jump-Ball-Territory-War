#include "terrain.h"


grid::grid(){
    this->Height = 0;
    this->mat = 0;
    this->mx = 0;
    this->mz = 0;
}

grid::grid(int mat,float inH, float inmx, float inmz){
    this->mat = mat;
    this->Height = inH;
    this->mx = inmx;
    this->mz = inmz;
}
