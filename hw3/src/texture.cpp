#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"
#include<iostream>

void Texture::loadTextureFromFile(const std::string &path) {
  int w, h, ch;
  stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
  //printf("diff = %d\n", ch);
  tex_data.assign(img, img + w * h * ch);
  stbi_image_free(img);
  this->w = w;
  this->h = h;
  this->ch = ch;
}

Texture::Texture()
{
    type = Texture::NONE;
}

Vec3f Texture::get_data(int index) {
    //std::cout<<"tex_data = "<<tex_data.size()<<std::endl;
    //std::cout<<"idx = "<<idx <<std::endl;
    return Vec3f((float)tex_data[index*3], (float)tex_data[index*3+1], (float)tex_data[index*3+2]);
    std::cout<<"get data success"<<std::endl;
}

Vec3f Texture::interpolation(float u, float v) {
    Vec2f pos;
    pos = Vec2f(u,v).cwiseProduct(Vec2f((float)w, (float)h));
    int x = (int)pos.x();
    int y = (int)pos.y();
    int u0 = std::min(x+1, w-1);
    int v0 = std::min(y+1, h-1);
    int left_min, left_max, right_min, right_max;
    left_min = x*h+y;
    left_max = x*h+v0;
    right_min = u0*h+y;
    right_max = u0*h+v0;
    Vec3f left_min_3 = get_data(left_min);
    Vec3f left_max_3 = get_data(left_max);
    Vec3f right_min_3 = get_data(right_min);
    Vec3f right_max_3 = get_data(right_max);
    float pixel_x;
    float pixel_y;
    if(u0 == x)
    {
        pixel_x = 0;
    }
    if(v0 == y)
    {
        pixel_y = 0;
    }
    if(u0 != x && v0 != y)
    {
        pixel_x = (u0 - pos.x())/((float)u0 - x);
        pixel_y = (v0 - pos.y())/((float)v0 - y);
    }
    Vec3f down,up;
    down = (1 - pixel_x) * left_min_3 + pixel_x * right_min_3;
    up = (1 - pixel_x) * left_max_3 + pixel_x * right_max_3;
    Vec3f res;
    res = ((1 - pixel_y) * down + pixel_y * up) / 255.f;
    return res;
}

