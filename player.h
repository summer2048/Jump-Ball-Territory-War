
#ifndef PLAYER_H
#define PLAYER_H

class player {
public:
    player();
    player(int inM, int Num, int inP, float px, float pz);
    int mat;
    int NumofMarble;
    int points;
    float basex;
    float basez;
};
#endif