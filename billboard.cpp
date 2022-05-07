#include "billboard.h"

#include "vendor/stb_image/stb_image.h"

#include "vec.h"
#include "mat.h"

#include "modelerdraw.h"
#include "modelerapp.h"
#include "camera.h"

#include "bitmap.h"
#include "drawbody.h"

Billboard::Billboard(char* file)
{
    stbi_set_flip_vertically_on_load(1);
    texture = stbi_load(file, &width, &height, &channel, 0);
}

Billboard::~Billboard()
{
    delete[] texture;
}

void Billboard::setTexture()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    if (channel == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
}

void Billboard::draw(float scale)
{
    Mat4f v = ModelerApplication::Instance()->GetCamera()->getCameraMat();
    Vec3f right(v[0][0], v[0][1], v[0][2]);
    Vec3f up(v[1][0], v[1][1], v[1][2]);

    glEnable(GL_TEXTURE_2D);

    Vec3f p0, p1, p2, p3;
    p0 = (right * -0.5f + up * -0.5) * scale;
    p1 = (right *  0.5f + up * -0.5) * scale;
    p2 = (right *  0.5f + up *  0.5) * scale;
    p3 = (right * -0.5f + up *  0.5) * scale;

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(p0[0], p0[1], p0[2]);
        glTexCoord2f(1, 0); glVertex3f(p1[0], p1[1], p1[2]);
        glTexCoord2f(1, 1); glVertex3f(p2[0], p2[1], p2[2]);
        glTexCoord2f(0, 1); glVertex3f(p3[0], p3[1], p3[2]);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}