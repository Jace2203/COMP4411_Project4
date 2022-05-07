#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec.h"
#include "billboard.h"

struct Particle
{
    Vec3f p;
    Vec3f v;
    Vec3f f;
    float m;

    float t;

    void draw(float t, bool bill = false, Billboard* b = nullptr);
};

#endif