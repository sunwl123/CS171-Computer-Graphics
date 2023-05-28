#ifndef TEXTURE_H
#define TEXTURE_H

#include "core.h"

class Texture
{
    Vec3f **image;
    int width;
    int height;
public:
    Texture(char *filename);
    void get_file(char *filename);
    Vec3f get_color(double u, double v);
};

class TextureMap
{
    Texture *texture;
    Vec3f color;
    double u1, u2, u3, u4;
    double v1, v2, v3, v4;
public:
    TextureMap(Texture *texture, double u1, double u2, double u3, double u4, double v1, double v2, double v3, double v4);
    TextureMap(Vec3f color);
    Vec3f get_color(Vec3f position);
};

#endif // TEXTURE_H