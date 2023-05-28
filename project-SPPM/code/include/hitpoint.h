#ifndef HITPOINT_H
#define HITPOINT_H
#include "core.h"
#include "brdf.h"

class HitPoint
{
public:
    Vec3f position;
    Vec3f direction;
    Vec3f normal;
    Vec3f color_weight;
    Vec3f color_flux;
    Vec3f color_flux_light;
    bool is_valid;
    int n;
    brdf brdf_type;
    double radius;
    HitPoint()
    {
        color_flux = Vec3f(0.0, 0.0, 0.0);
        color_flux_light = Vec3f(0.0, 0.0, 0.0);
        radius = 1e-5;
        n = 0;
        is_valid = false;
    }
};

class KDTreeNode
{
public:
    HitPoint *hitpoint;
    Vec3f left;
    Vec3f right;
    KDTreeNode *left_child;
    KDTreeNode *right_child;
    double max_radius;
};

class KDTree
{
    int n;
    HitPoint **hitpoints;
    KDTreeNode *build(int l, int r, int depth);
    void erase(KDTreeNode *node);

public:
    KDTreeNode *root;
    KDTree(std::vector<HitPoint*>* hitpoints);
    ~KDTree();
    void update(KDTreeNode *node, Vec3f photon, Vec3f color_weight1, Vec3f direction);
};

#endif // HITPOINT_H