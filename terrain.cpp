#include "terrain.h"


grid::grid(){
    this->Height = 0;
    this->color = colour(0,0,0);
    this->mx = 0;
    this->mz = 0;
}

grid::grid(colour Col,float inH, float inmx, float inmz){
    this->color = Col;
    this->Height = inH;
    this->mx = inmx;
    this->mz = inmz;
}
