#ifndef RAY_H
#define RAY_H
#include "core.h"
#include <math.h>

class Ray
{
public:
    Vec3f point_s;
    Vec3f vector_d;
    Ray(Vec3f point_s, Vec3f vector_d)
    {
        this->point_s = point_s;
        float sqrt = sqrtf(vector_d[0] * vector_d[0] + vector_d[1] * vector_d[1] + vector_d[2] * vector_d[2]);
        this->vector_d = vector_d / sqrt;
    }

};

#endif
