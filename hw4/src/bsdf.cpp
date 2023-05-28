#include "bsdf.h"
#include "utils.h"

#include <utility>

Vec3f global_up(0.f, 0.f, 1.f);

IdealDiffusion::IdealDiffusion(const Vec3f &color) : color(color) {}

Vec3f IdealDiffusion::evaluate(Interaction &interaction) const
{
    return color * INV_PI;
}

float IdealDiffusion::pdf(Interaction &interaction) const {
    float cosine = (interaction.wo).dot(interaction.normal);
    return cosine * INV_PI;
}

float IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
    Vec2f temp = sampler.get2D();
    float theta = asin(sqrt(temp.x()));
    float phi = 2 * PI*temp.y();
    Vec3f dir(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    Mat3f glb = Eigen::Quaternionf::FromTwoVectors(global_up, interaction.normal).toRotationMatrix();
    interaction.wo = (glb * dir).normalized();
    return pdf(interaction);
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
    return false;
}

IdealSpecular::IdealSpecular(const Vec3f &color) : color(color) {}

Vec3f IdealSpecular::evaluate(Interaction &interaction) const {
    return color;
}

float IdealSpecular::pdf(Interaction &interaction) const {
    return 1.0f;
}
float IdealSpecular::sample(Interaction &interaction, Sampler &sampler) const {
    float cosine = (-interaction.wi).dot(interaction.normal);
    interaction.wo = (2 * cosine * interaction.normal + interaction.wi).normalized();
    return pdf(interaction);
}
bool IdealSpecular::isDelta() const {
    return true;
}



float FrDielectric(float cosThetaI, float etaI, float etaT) {
    cosThetaI = std::clamp(cosThetaI, -1.f, 1.f);
    bool entering = cosThetaI > 0.f;
    if (!entering)
    {
        std::swap(etaI, etaT);
        cosThetaI = std::abs(cosThetaI);
    }
    float sinThetaI = std::sqrt(std::max(0.f, 1 - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;
    float cosThetaT = std::sqrt(std::max(0.f, 1 - sinThetaT * sinThetaT));
    float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
    float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
    return (Rparl * Rparl + Rperp * Rperp) / 2;
}

bool Refract(const Vec3f &wi, const Vec3f &n, float eta, Vec3f *wo)
{
    float cosThetaI = std::abs(n.dot(wi));
    float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
    float sin2ThetaT = eta * eta * sin2ThetaI;
    if (sin2ThetaT >= 1.f)
    {
        return false;
    }
    float cosThetaT = std::sqrt(1 - sin2ThetaT);
    *wo = ((eta * cosThetaI - cosThetaT) * n + eta * (-wi)).normalized();
    return true;
}

Translucent::Translucent(const Vec3f &color) : color(color) {}

Translucent::Translucent(const Vec3f &color, float eta_in, float eta_out) : color(color), eta_in(eta_in), eta_out(eta_out) {}
Vec3f Translucent::evaluate(Interaction &interaction) const {
    float cosine = (-interaction.wi).dot(interaction.normal);
    Vec3f reflect = (2 * cosine * interaction.normal + interaction.wi).normalized();
    if (interaction.wo.dot(reflect) >= 0.999)
    {
        return color;
    }
    else {
        float etaI, etaT;
        if(cosine > 0)
        {
            etaI = eta_in;
            etaT = eta_out;
        }
        else
        {
            etaI = eta_out;
            etaT = eta_in;
        }
        float dielectric = FrDielectric(cosine, etaI, etaT);
        float temp = (etaI * etaI) / (etaT * etaT);
        return temp * (1 - dielectric) * color;
    }
}
float Translucent::pdf(Interaction &interaction) const
{
    return 1.0f;
}

float Translucent::sample(Interaction &interaction, Sampler &sampler) const {
    float cosine = (-interaction.wi).dot(interaction.normal);
    float etaI, etaT;
    if(cosine > 0)
    {
        etaI = eta_in;
        etaT = eta_out;
    }
    else
    {
        etaI = eta_out;
        etaT = eta_in;
    }
    bool refracted = Refract(-interaction.wi, interaction.normal, etaI / etaT, &interaction.wo);
    if (refracted == false)
    {
        interaction.wo = (2 * cosine * interaction.normal + interaction.wi).normalized();
    }
    return pdf(interaction);
}
bool Translucent::isDelta() const
{
    return true;
}
