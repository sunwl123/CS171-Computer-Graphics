#ifndef BRDF_H
#define BRDF_H

#include "core.h"
#include "ray.h"

class brdf
{
public:
    double specular;
    double diffuse;
    double refraction;
    double refract_index;
    double d;
    double s;
    double phone_lighting;
    brdf() {}
    brdf(double specular, double diffuse, double refraction, double d, double s, double phone_lighting, double refract_index)
        : specular(specular), diffuse(diffuse), refraction(refraction), d(d), s(s), phone_lighting(phone_lighting), refract_index(refract_index)
    {
    }
};

enum
{
    DIFFUSE,
    MIRROR,
    GLASS,
    LIGHT,
    MARBLE,
    FLOOR,
    WALL,
    DESK,
    STANFORD_MODEL,
    WATER,
    TEAPOT
};

const brdf brdf_settings[] = {
    brdf(0.0, 1.0, 0.0, 0.7, 0.0, 0.0, 0.0),  // DIFFUSE
    brdf(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),  // MIRROR
    brdf(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.65), // GLASS
    brdf(0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0),  // LIGHT
    brdf(0.1, 0.9, 0.0, 1.0, 0.0, 50.0, 0.0), // MARBLE
    brdf(0.1, 0.9, 0.0, 0.9, 0.1, 50.0, 0.0), // FLOOR
    brdf(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0),  // WALL
    brdf(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0),  // DESK
    brdf(0.0, 1.0, 0.0, 0.9, 0.1, 10.0, 1.0), // STANFORD_MODEL
    brdf(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.3),  // WATER
    brdf(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.5),  // TEAPOT
};

#endif