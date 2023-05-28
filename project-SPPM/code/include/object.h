#ifndef OBJECT_H
#define OBJECT_H

#include "core.h"
#include "Config.h"
#include "texture.h"
#include "brdf.h"

using namespace std;

class Object;

class Face
{
public:
    Object *object;
    TextureMap *texture;
    int BRDF;
    virtual Vec3f min() = 0;
    virtual Vec3f max() = 0;
    virtual Vec3f center() = 0;
    virtual void scale(
        double x1, double x2, double x3, double x4,
        double y1, double y2, double y3, double y4,
        double z1, double z2, double z3, double z4) = 0;
    virtual pair<double, Vec3f> intersect(Ray ray) = 0;
};

enum
{
    TRIANGLE,
    SPHERE,
    BEZIER
};

class Triangle : public Face
{
public:
    Vec3f *p1, *p2, *p3;
    Triangle(Vec3f *p1, Vec3f *p2, Vec3f *p3, TextureMap *texture = nullptr, int BRDF = 0)
    {
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
        this->texture = texture;
        this->BRDF = BRDF;
    }
    Vec3f min();
    Vec3f max();
    Vec3f center();
    void scale(
        double x1, double x2, double x3, double x4,
        double y1, double y2, double y3, double y4,
        double z1, double z2, double z3, double z4)
    {
    }
    pair<double, Vec3f> intersect(Ray ray);
    double intersect_plane(Ray ray);
};

class SphereF : public Face
{
public:
    Vec3f point;
    double radius;
    SphereF(Vec3f point, double radius, TextureMap *texture, int BRDF)
    {
        this->point = point;
        this->radius = radius;
        this->texture = texture;
        this->BRDF = BRDF;
    }
    Vec3f min();
    Vec3f max();
    Vec3f center();
    void scale(
        double x1, double x2, double x3, double x4,
        double y1, double y2, double y3, double y4,
        double z1, double z2, double z3, double z4)
    {
    }
    pair<double, Vec3f> intersect(Ray ray);
};

class Disc : public Face
{
public:
    Vec3f point;
    double radius;
    Disc(Vec3f point, double radius, TextureMap *texture, int BRDF)
    {
        this->point = point;
        this->radius = radius;
        this->texture = texture;
        this->BRDF = BRDF;
    }
    Vec3f min();
    Vec3f max();
    Vec3f center();
    void scale(
        double x1, double x2, double x3, double x4,
        double y1, double y2, double y3, double y4,
        double z1, double z2, double z3, double z4)
    {
    }
    pair<double, Vec3f> intersect(Ray ray);
};

class Object
{
public:
    Vec3f **vertexes;
    Vec3f *center;
    Face **faces;
    int faces_count;
    int vertexes_count;
    Object()
    {
        center = nullptr;
    }
    void import_ply(char *filename, TextureMap *texture, int BRDF);
    void import_bpt(char *filename, TextureMap *texture, int BRDF);
    void calculate_center();
    void print_box();
    void scale(
        double x1, double x2, double x3, double x4,
        double y1, double y2, double y3, double y4,
        double z1, double z2, double z3, double z4);
    void rotate(double angle);
};

class Sphere : public Object
{
public:
    Sphere(Vec3f c, double radius, TextureMap *texture, int BRDF);
};

#endif