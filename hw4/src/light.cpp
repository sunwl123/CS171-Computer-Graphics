#include "light.h"
#include "utils.h"

Light::Light(const Vec3f &pos, const Vec3f &color) :
        position(pos), radiance(color) {}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &size) :
        Light(pos, color), size(size) {
    Vec3f v1, v2, v3, v4;
    v1 = pos + Vec3f(size.x() / 2, 0.f, -size.y() / 2);
    v2 = pos + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
    v3 = pos + Vec3f(-size.x() / 2, 0.f, size.y() / 2);
    v4 = pos + Vec3f(size.x() / 2, 0.f, size.y() / 2);
    light_mesh = TriangleMesh({v1, v2, v3, v4}, {Vec3f(0, -1, 0)}, {0, 1, 2, 0, 2, 3}, {0,0,0,0,0,0});
}

Vec3f SquareArea_normal(0.f, -1.f, 0.f);

Vec3f SquareAreaLight::emission(const Vec3f &pos, const Vec3f &dir) const {
    float cosine = dir.dot(SquareArea_normal);
    float light_cosine;
    if(cosine > 0.f)
    {
        light_cosine = cosine;
    }
    else light_cosine = 0.f;
    return light_cosine * radiance;
}

float SquareAreaLight::pdf(const Interaction &interaction, Vec3f pos) {
    float cosine = -interaction.wo.dot(SquareArea_normal);
    float light_cosine;
    if(cosine > 0.f)
    {
        light_cosine = cosine;
    }
    else light_cosine = 0.f;
    float distance = (pos - interaction.pos).norm();
    return light_cosine / (distance * distance);
}

Vec3f SquareAreaLight::sample(Interaction &interaction, float *pdf, Sampler &sampler) const {
    Vec2f pos_u = sampler.get2D();
    Vec3f pos_sample = position + Vec3f(pos_u.x()-0.5f, 0.f, pos_u.y()-0.5f).cwiseProduct(Vec3f(size.x(), 0.f, size.y()));
    *pdf = 1.f/(size.x()*size.y());
    interaction.wo = (pos_sample - interaction.pos).normalized();
    return pos_sample;
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    if (light_mesh.intersect(ray, interaction)) {
        interaction.type = Interaction::Type::LIGHT;
        return true;
    }
    return false;
}

