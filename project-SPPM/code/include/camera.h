#ifndef CAMERA_H
#define CAMERA_H
#include "scene.h"

class Camera
{
    int width;
    int height;
    double fx, fy;
    double aperture, focus;
    Vec3f light;
    Vec3f **ground;
    Scene *scene;
    Vec3f position;
    vector<HitPoint *> *hitpoints;
    void evaluate(int rounds, int photon_num);
public:
    Camera(int width, int height)
    {
        this->width = width;
        this->height = height;
        ground = new Vec3f*[width];
        for (int i = 0; i < width; i++)
        {
            ground[i] = new Vec3f[height];
        }
    }
    void set_scene(Scene *scene)
    {
        this->scene = scene;
    }
    void set_position(Vec3f position)
    {
        this->position = position;
    }
    void set_length(double fx, double fy, double aperture, double focus)
    {
        this->fx = fx;
        this->fy = fy;
        this->aperture = aperture;
        this->focus = focus;
    }
    void render(int rounds, int photon_num = 20480);
    void save_pic(char *filename);
};



#endif