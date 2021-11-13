#include "player.h"

player::player(){
    this->mat = 0;
    this->NumofMarble = 0;
    this->points = 0;
    this->basex = 0;
    this->basez = 0;
}

player::player(int inM, int Num, int inP, float px, float pz){
    this->mat = inM;
    this->NumofMarble = Num;
    this->points = inP;
    this->basex = px;
    this->basez = pz;
}