#include "vec.h"

#ifndef METABALL_H
#define METABALL_H

enum {
    META_NORMAL = 1,
    META_INVERSE = -1
};

class Metaball
{
public:
    Metaball(Vec3d& center, double radius, int inv = META_NORMAL);
    ~Metaball();

    Vec3d*  getCenter() const;
    void    setCenter(Vec3d& center);

    double  getRadius() const;
    void    setRadius(double radius);

    double  getValue(const Vec3d& vec) const;

    double  getX() const;
    double  getY() const;
    double  getZ() const;
private:
    Vec3d*  c;
    double  r;
    int     inv;
};

class MetaballContainer
{
public:
    MetaballContainer(int n, double threshold, double size);
    ~MetaballContainer();

    void append(Metaball* metaball);

    void setThreshold(double threshold);

    Metaball* operator[](int nth);

    void render();
private:
    int         num;
    Metaball**  metaballs;

    int         count;

    double      gridSize;
    double      threshold;

    Vec3d*      getMinMax();

    double      getValue(const Vec3d& vec);

    static const int edgeTable[256];
    static const int triTable[256][16];

    Vec3d VertexInterp(Vec3d p1, Vec3d p2, double v1, double v2);
};

#endif