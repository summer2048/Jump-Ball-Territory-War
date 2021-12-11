#include "particle.h"
#include <math.h>

Particle::Particle(float pX, float pY, float pZ, float dX, float dY, float dZ)
    : position{pX,pY,pZ}
    , direction{dX, dY,dZ}
    , is_permanent(false)
{}