#include "particle.h"

#include "modelerdraw.h"
#include "drawbody.h"

void Particle::draw(float tt)
{
    glPushMatrix();

        glTranslatef(p[0], p[1], p[2]);
        setDiffuseColor((tt - t) / 2.5f, (tt - t) / 2.5f, 1.0f);

        glScaled(m * 0.02f, m * 0.02f, m * 0.02f);
        callList(7);

    glPopMatrix();
}