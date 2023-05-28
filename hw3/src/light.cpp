#include "light.h"
#include <iostream>
constexpr uint32_t SAMPLE_NUM_x = 16;
constexpr uint32_t SAMPLE_NUM_y = 8;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), color(color) {

}
Vec3f Light::getColor() const {
  return color;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
    Light(pos, color),
    rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
    std::vector<LightSample> samples;
    // TODO: Your code here.
    Vec2f rec_s = rectangle.getSize();
    Vec3f tang = rectangle.getTangent().normalized();
    Vec3f cotg = rectangle.getNormal().cross(tang).normalized();
    for(int x=0;x<SAMPLE_NUM_x;x++) {
        for (int y=0;y<SAMPLE_NUM_y;y++) {
            float pos_x,pos_y;
            pos_x = (2*((float)x+0.5f)/SAMPLE_NUM_x-1) * rec_s.x()/2;
            pos_y = (2*((float)y+0.5f)/SAMPLE_NUM_y-1) * rec_s.y()/2;
            LightSample s;
            s.color = color;
            s.position = position + pos_x * tang + pos_y * cotg;
            samples.push_back(s);
        }
        
    }
    return samples;
}
bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.
    if (rectangle.intersect(ray, interaction))
    {
        interaction.type = Interaction::Type::LIGHT;
        return true;
    }
    return false;
}

