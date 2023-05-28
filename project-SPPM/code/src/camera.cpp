#include "camera.h"
#include "core.h"
#include "utils.h"

void Camera::evaluate(int rounds, int photon_num)
{
    // evaluate the scene
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            HitPoint *hitpoint;
            hitpoint = (*hitpoints)[i * height + j];
            ground[i][j] = hitpoint->color_flux / (PI * hitpoint->radius * rounds * photon_num) + light.cwiseProduct(hitpoint->color_flux_light) / rounds;
            // printf("flux = %f\n", hitpoint->color_flux);
            ground[i][j].x() = sqrtf(ground[i][j].x());
            ground[i][j].y() = sqrtf(ground[i][j].y());
            ground[i][j].z() = sqrtf(ground[i][j].z());
            // ground[i][j].y() = std::pow(ground[i][j].y(), 0.5);
            // ground[i][j].z() = std::pow(ground[i][j].z(), 0.5);
            // if (ground[i][j].x() > 1.0)
            //     ground[i][j].x() = 1.0;
            // if (ground[i][j].y() > 1.0)
            //     ground[i][j].y() = 1.0;
            // if (ground[i][j].z() > 1.0)
            //     ground[i][j].z() = 1.0;
            ground[i][j] = minVec3f(ground[i][j], Vec3f(1, 1, 1));
        }
    }
}

void Camera::render(int rounds, int photon_num)
{
    // render the scene
    scene->init_objectKDTree();
    int w = width / 2;
    int h = height / 2;
    float coordinate = position.z() + focus;
    Triangle plane(new Vec3f(0, 0, coordinate), new Vec3f(0, 1, coordinate), new Vec3f(1, 0, coordinate));
    hitpoints = new vector<HitPoint *>;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            hitpoints->push_back(new HitPoint);
        }
    }
    light = Vec3f(1.0f, 1.0f, 1.0f);
    const Vec3f init_weight = Vec3f(2.5f, 2.5f, 2.5f);
    for (int round = 0; round < rounds; ++round)
    {
        printf("Round %d/%d:\n", round + 1, rounds);
        for (int i = 0; i < width; i++)
        {
            printf("\r%.3lf%%", (float)i * 100.0 / width);
            // printf("faq render %d, %d\n", i, width); // i=256
#pragma omp parallel for schedule(dynamic, 60), num_threads(8)
            for (int j = 0; j < height; j++)
            {
                // if (i == 256)
                // {
                //     printf("faq render j = %d, %d\n", j, height);
                //     printf("len(hps) = %d, my = %d\n", hitpoints->size(), 256 * 768);
                // }
                Vec3f p(float(i - w) / width / fx, float(j - h) / height / fy, 0.0f);
                Ray ray(position, p - position);
                double t = plane.intersect_plane(ray);
                Vec3f point = ray.point_s + ray.vector_d * t;
                double theta = Utils::my_random(0.0f, 2.0f * PI);
                ray.point_s = ray.point_s + Vec3f(cos(theta), sin(theta), 0.0f) * aperture;
                ray.vector_d = point - ray.point_s;
                ray.vector_d.normalize();
                (*hitpoints)[i * height + j]->is_valid = false;
                (*hitpoints)[i * height + j]->direction = ray.vector_d * -1;
                scene->ray_trace(ray, Vec3f(1.0f, 1.0f, 1.0f), 1,
                                 (long long)round * (photon_num + width * height) + i * height + j, (*hitpoints)[i * height + j]);
            }
        }
        printf("\r100.00%%\n");
        scene->init_hitKDTree(hitpoints);
#pragma omp parallel for schedule(dynamic, 128), num_threads(8)
        for (int i = 0; i < photon_num; ++i)
        {
            Ray ray = scene->generate_ray((long long)round * photon_num + (round + 1) * width * height + i);
            scene->ray_trace(ray, init_weight.cwiseProduct(light), 1,
                             (long long)round * photon_num + i);
        }
        printf("\rPhoton tracing finished.\n");

        if ((round + 1) % Config::checkpoint_interval == 0)
        {
            evaluate(round + 1, photon_num);
            char filename[100];
            sprintf(filename, "checkpoint-%d.ppm", round + 1);
            save_pic(filename);
        }
    }
    evaluate(rounds, photon_num);
}

void Camera::save_pic(char *filename)
{
    // save the picture
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "P3\n%d %d\n255\n", width, height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            fprintf(fp, "%d %d %d ", (int)(ground[j][height - i - 1].x() * 255.0f + 0.5f), (int)(ground[j][height - i - 1].y() * 255.0f + 0.5f), (int)(ground[j][height - i - 1].z() * 255.0f + 0.5f));
            // fprintf(fp, "%f %f %f ", ground[j][i].x(), ground[j][i].y(), ground[j][i].z());
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
    fprintf(stderr, "Image saved to %s\n", filename);
}