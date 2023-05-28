#include "texture.h"

Texture::Texture(char *filename)
{
    get_file(filename);
}

void Texture::get_file(char *filename)
{
    // get the texture file
    FILE *file = fopen(filename, "r");
    fscanf(file, "%*s%d%d%*d", &width, &height);
    image = new Vec3f*[height];
    for(int i=0;i<height;++i)
    {
        image[i] = new Vec3f[width];
        for(int j=0;j<width;++j)
        {
            int r, g, b;
            fscanf(file, "%d%d%d", &r, &g, &b);
            image[i][j] = Vec3f((float)r/255.f, (float)g/255.f, (float)b/255.f);
        }
    }
    fclose(file);
}

Vec3f Texture::get_color(double u, double v)
{
    // get the texture color
    int u1 = u * height;
    int v1 = v * width;
    if(u1 >= 0 && u1 < height && v1 >= 0 && v1 < width)
    {
        return image[u1][v1];
    }
    else
    {
        return Vec3f(0, 0, 0);
    }
}

TextureMap::TextureMap(Texture *texture, double u1, double u2, double u3, double u4, double v1, double v2, double v3, double v4)
{
    // init the texture map
    this->texture = texture;
    this->u1 = u1;
    this->u2 = u2;
    this->u3 = u3;
    this->u4 = u4;
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
    this->v4 = v4;
}

TextureMap::TextureMap(Vec3f color)
{
    // init the texture map
    this->texture = nullptr;
    this->color = color;
}

Vec3f TextureMap::get_color(Vec3f position)
{
    // get the texture map color
    if(texture == nullptr)
    {
        return color;
    }
    double x = position.x()*u1 + position.y()*u2 + position.z()*u3 + u4;
    double y = position.x()*v1 + position.y()*v2 + position.z()*v3 + v4;
    return texture->get_color(x, y);
}