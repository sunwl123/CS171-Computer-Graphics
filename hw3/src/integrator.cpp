#include "integrator.h"
#include <omp.h>

#include <utility>
#include <iostream>
Vec3f vec3f_mul(Vec3f a, Vec3f b)
{
    Vec3f res;
    res = Vec3f (a.x() * b.x() , a.y() * b.y() , a.z() * b.z());
    return res;
}


PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
        : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
    int sample_num = 20;
    std::vector<LightSample> light_samples;
    light_samples = scene->getLight()->samples();
    srand(0);
#pragma omp parallel for schedule(guided, 2), shared(cnt)
    for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0.0f, 0.0f, 0.0f);
            // TODO: Your code here.
            for(int i=0;i<=sample_num;i++) {
                float pos_x,pos_y;
                pos_x = dx + (float)rand()/RAND_MAX - 0.5f;
                pos_y = dy + (float)rand()/RAND_MAX - 0.5f;
                Ray ray = camera->generateRay(pos_x, pos_y);
                Interaction interaction;
                //bool hit = scene->intersect(ray, interaction);
                //std::cout<<"faq"<<std::endl;
                Vec3f radiance_ray(0.0f, 0.0f, 0.0f);
                if(scene->intersect(ray, interaction)){
                    // std::printf("%f\n", interaction.dist);
                    if(interaction.type == Interaction::Type::LIGHT){
                        radiance_ray = scene->getLight()->getColor();
                    }
                    else{
                        for(auto light:light_samples){
                            Ray ref(interaction.pos + 0.001 *interaction.normal, (light.position - interaction.pos).normalized());
                            Interaction pos_light;
                            if (scene->intersect(ref, pos_light)&&pos_light.type==Interaction::Type::LIGHT) {
                                radiance_ray =  radiance_ray + radiance(ray, interaction, light);
                            }
                            else {
                                radiance_ray =  radiance_ray + vec3f_mul(scene->getAmbient(), interaction.model.ambient);
                            }
                        }
                        radiance_ray /= light_samples.size();
                    }
                    L = L + radiance_ray;
                }

            }
            L /= sample_num;
            // std::printf("%d %d %f %f %f\n", dx, dy, L(0), L(1), L(2));
            camera->getImage()->setPixel(dx, dy, L);
        }
    }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction, LightSample light) const {
    Vec3f radiance(0.0f, 0.0f, 0.0f);

    //Ambient
    Vec3f ambient = scene->getAmbient();

    //Diffuse
    Vec3f normal_direction = interaction.normal.normalized();
    Vec3f light_direction = (light.position - interaction.pos).normalized();
    float diffuse;
    diffuse = std::max(0.0f, normal_direction.dot(light_direction));

    //HighLight
    Vec3f view_direction = -ray.direction;
    Vec3f length = 2*(-light_direction.dot(-normal_direction))*(-normal_direction)+light_direction;
    Vec3f reflect_direction = (light_direction + view_direction).normalized();
    float specular;
    specular = std::pow(std::max(view_direction.dot(-length), 0.0f), interaction.model.shininess);

    Vec3f color;
    color = diffuse*interaction.model.diffusion + specular*interaction.model.specular;
    radiance = vec3f_mul(ambient, interaction.model.ambient) + vec3f_mul(color, light.color);

    return radiance;

}