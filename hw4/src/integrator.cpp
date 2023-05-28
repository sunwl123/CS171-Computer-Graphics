#include "integrator.h"
#include "utils.h"
#include <omp.h>

#include <utility>

Integrator::Integrator(std::shared_ptr<Camera> cam,
                       std::shared_ptr<Scene> scene, int spp, int max_depth)
        : camera(std::move(cam)), scene(std::move(scene)), spp(spp), max_depth(max_depth)
{
}

void Integrator::render() const
{
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
    Sampler sampler;
#pragma omp parallel for schedule(dynamic), shared(cnt), private(sampler)
    for (int dx = 0; dx < resolution.x(); dx++)
    {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        sampler.setSeed(omp_get_thread_num());
        for (int dy = 0; dy < resolution.y(); dy++)
        {
            Vec3f L(0, 0, 0);
            // TODO: generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.
            for (int i = 0;i < this->spp; ++i)
            {
                Vec2f direction = sampler.get2D();
                Ray r = camera->generateRay((float)dx + direction.x(), (float)dy+direction.y());
                L += radiance(r, sampler);
            }
            L /= spp;
            camera->getImage()->setPixel(dx, dy, L);
        }
    }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler) const
{
    Vec3f L(0, 0, 0);
    Vec3f beta(1, 1, 1);
    bool isDelta = false;
    for (int i = 0; i < max_depth; ++i)
    {
        Interaction I_i;
        bool hit = scene->intersect(ray, I_i);
        I_i.wi = ray.direction;
        if (hit){
            if (I_i.type == Interaction::Type::LIGHT && i == 0) {
                L = scene->getLight()->emission(I_i.pos, -I_i.wi);
                break;
            }
            else if (I_i.type == Interaction::Type::GEOMETRY && I_i.material->isDelta()) {
                float pdf_obj = I_i.material->sample(I_i, sampler);
                Vec3f _brdf = I_i.material->evaluate(I_i);

                ray = Ray(I_i.pos + 0.001 * I_i.wo, I_i.wo);
                Interaction I_reflect;
                bool reflect_hit = scene->intersect(ray, I_reflect);
                I_reflect.wi = ray.direction;

                if (reflect_hit && I_reflect.type == Interaction::Type::LIGHT && (i == 0 || this->spp >= 1024))
                {
                    Vec3f _light = scene->getLight()->emission(I_reflect.pos, -I_reflect.wi);
                    L += beta.cwiseProduct(_light.cwiseProduct(_brdf));
                    break;
                }
                beta = beta.cwiseProduct(_brdf);
            }
            else if (I_i.type == Interaction::Type::GEOMETRY && !I_i.material->isDelta())
            {
                Vec3f direct_light = directLighting(I_i, sampler);
                L += beta.cwiseProduct(direct_light);
                float pdf = I_i.material->sample(I_i, sampler);
                float cosine = I_i.wo.dot(I_i.normal);
                Vec3f brdf = I_i.material->evaluate(I_i);
                beta = beta.cwiseProduct(brdf) * cosine / pdf;
                ray = Ray(I_i.pos + 0.001 * I_i.wo, I_i.wo);
            }
        }
        else
        {
            break;
        }
    }
    return L;
}

Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const
{
    Vec3f L(0, 0, 0);
    // Compute direct lighting.
    float pdf;
    Vec3f pos;
    std::shared_ptr<Light> light = scene->getLight();
    pos = light->sample(interaction, &pdf, sampler);
    Ray light2(interaction.pos + 0.001 * interaction.wo, interaction.wo);

    if(!scene->isShadowed(light2))
    {
        Vec3f _light = light->emission(pos, -interaction.wo);
        Vec3f _brdf = interaction.material->evaluate(interaction);
        float cosine = interaction.wo.dot(interaction.normal);
        float ppdf = light->pdf(interaction, pos);
        L = _light.cwiseProduct(_brdf) * cosine * ppdf / pdf;
    }

    return L;
}