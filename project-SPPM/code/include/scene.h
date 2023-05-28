#ifndef SCENE_H
#define SCENE_H
#include "core.h"
#include "object.h"
#include "ray.h"
#include "hitpoint.h"

class ObjectKDTreeNode
{
public:
    Vec3f min;
    Vec3f max;
    vector<Face *> *faces;
    ObjectKDTreeNode *left_child;
    ObjectKDTreeNode *right_child;
    int left;
    int right;
    bool if_leaf(Face *face);
};

class ObjectKDTree
{
    int n;
    Face **vertexes;
    ObjectKDTreeNode *build(int depth, int d, vector<Face *> *faces, Vec3f min, Vec3f max);
    void get_faces(ObjectKDTreeNode *node, vector<Face *> *faces);

public:
    ObjectKDTreeNode *root;
    vector<Face *> *faces;
    ObjectKDTree(vector<Face *> *faces);
    ~ObjectKDTree();
    double intersect_node(ObjectKDTreeNode *node, Ray ray);
    void intersect_faces(ObjectKDTreeNode *node, Ray ray, Face *&face, double &tmin, Vec3f &normal);
};

class Scene
{
    vector<Object *> objects;
    vector<HitPoint *> hitpoints;
    KDTree *hitKDTree;
    ObjectKDTree *objectKDTree;
    Vec3f Points;
    Vec3f Normals;
    double radius;
    Vec3f sampleRay(Vec3f normal, int depth, long long step, double s = 1.0);

public:
    void add_object(Object *object);
    Scene(Vec3f Points, double radius, Vec3f Normals) : Points(Points), radius(radius), Normals(Normals)
    {
        hitKDTree = nullptr;
    }
    Ray generate_ray(long long step);
    void ray_trace(const Ray &ray, const Vec3f &color_weight, int depth, long long step, HitPoint *hitpoint = nullptr);
    void init_hitKDTree(vector<HitPoint *> *hitpoints);
    void init_objectKDTree();
};

#endif // SCENE_H
