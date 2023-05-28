#include "hitpoint.h"

bool cmpx(HitPoint *a, HitPoint *b)
{
    return a->position.x() < b->position.x();
}

bool cmpy(HitPoint *a, HitPoint *b)
{
    return a->position.y() < b->position.y();
}

bool cmpz(HitPoint *a, HitPoint *b)
{
    return a->position.z() < b->position.z();
}

KDTreeNode *KDTree::build(int l, int r, int depth)
{
    // build the kd-tree
    KDTreeNode *p = new KDTreeNode;
    p->left = Vec3f(1e10, 1e10, 1e10);
    p->right = Vec3f(-1e10, -1e10, -1e10);
    p->max_radius = 0.0;
    for (int i = l; i <= r; ++i)
    {
        p->left = minVec3f(p->left, hitpoints[i]->position);
        p->right = maxVec3f(p->right, hitpoints[i]->position);
        p->max_radius = std::max(p->max_radius, hitpoints[i]->radius);
    }
    int mid = (l + r) >> 1;
    if (depth == 0)
    {
        std::nth_element(hitpoints + l, hitpoints + mid, hitpoints + r + 1, cmpx);
    }
    else if (depth == 1)
    {
        std::nth_element(hitpoints + l, hitpoints + mid, hitpoints + r + 1, cmpy);
    }
    else
    {
        std::nth_element(hitpoints + l, hitpoints + mid, hitpoints + r + 1, cmpz);
    }
    p->hitpoint = hitpoints[mid];
    if (l <= mid - 1)
    {
        p->left_child = build(l, mid - 1, (depth + 1) % 3);
    }
    else
    {
        p->left_child = nullptr;
    }
    if (mid + 1 <= r)
    {
        p->right_child = build(mid + 1, r, (depth + 1) % 3);
    }
    else
    {
        p->right_child = nullptr;
    }
    return p;
}

void KDTree::update(KDTreeNode *node, Vec3f photon, Vec3f color_weight1, Vec3f direction)
{
    // update the kd-tree
    if (node == nullptr)
    {
        return;
    }
    double min_dist = 0.0;
    double max_dist = 0.0;
    if (photon.x() > node->right.x())
    {
        min_dist += (photon.x() - node->right.x()) * (photon.x() - node->right.x());
    }
    if (photon.x() < node->left.x())
    {
        min_dist += (node->left.x() - photon.x()) * (node->left.x() - photon.x());
    }
    if (photon.y() > node->right.y())
    {
        min_dist += (photon.y() - node->right.y()) * (photon.y() - node->right.y());
    }
    if (photon.y() < node->left.y())
    {
        min_dist += (node->left.y() - photon.y()) * (node->left.y() - photon.y());
    }
    if (photon.z() > node->right.z())
    {
        min_dist += (photon.z() - node->right.z()) * (photon.z() - node->right.z());
    }
    if (photon.z() < node->left.z())
    {
        min_dist += (node->left.z() - photon.z()) * (node->left.z() - photon.z());
    }
    if (min_dist > node->max_radius)
    {
        return;
    }
    if (node->hitpoint->is_valid == true)
    {
        // printf("update::valid = faq\n");
        if ((photon - node->hitpoint->position).norm() * (photon - node->hitpoint->position).norm() <= node->hitpoint->radius) // radius square
        {
            // printf("update::inside = faq\n");
            HitPoint *hp = node->hitpoint;
            double factor = (hp->n * 0.7 + 0.7) / (hp->n * 0.7 + 1.0);
            Vec3f dr = direction - hp->normal * (2 * direction.dot(hp->normal));
            double rho;
            // printf("update:: hp->brdf_type.d = %f, hp->brdf_type.s = %f, * = %f\n", hp->brdf_type.d, hp->brdf_type.s, std::pow(dr.dot(hp->direction), hp->brdf_type.phone_lighting));
            rho = hp->brdf_type.d + hp->brdf_type.s * std::pow(dr.dot(hp->direction), hp->brdf_type.phone_lighting);
            // printf("update::rho = %f\n", rho);
            if (rho < 0)
            {
                rho = 0;
            }
            else if (rho > 1)
            {
                rho = 1;
            }
            hp->n++;
            hp->radius = hp->radius * factor; // sqrt factor
            hp->color_flux = factor * (hp->color_flux + hp->color_weight.cwiseProduct(color_weight1) * rho);
            // printf("update::color = %f, %f, %f\n", hp->color_flux.x(), hp->color_flux.y(), hp->color_flux.z());
        }
    }
    if (node->left_child != nullptr)
    {
        update(node->left_child, photon, color_weight1, direction);
    }
    if (node->right_child != nullptr)
    {
        update(node->right_child, photon, color_weight1, direction);
    }
    node->max_radius = node->hitpoint->radius;
    if (node->left_child != nullptr && node->left_child->hitpoint->radius > node->max_radius)
    {
        node->max_radius = node->left_child->hitpoint->radius;
    }
    if (node->right_child != nullptr && node->right_child->hitpoint->radius > node->max_radius)
    {
        node->max_radius = node->right_child->hitpoint->radius;
    }
}

KDTree::~KDTree()
{
    if (root == nullptr)
    {
        return;
    }
    erase(root);
    delete[] hitpoints;
}

KDTree::KDTree(std::vector<HitPoint *> *hitpoints)
{
    // init the tree root
    n = hitpoints->size();
    this->hitpoints = new HitPoint *[n];
    for (int i = 0; i < n; ++i)
    {
        this->hitpoints[i] = (*hitpoints)[i];
    }
    root = build(0, n - 1, 0);
}

void KDTree::erase(KDTreeNode *node)
{
    // erase the kd-tree node
    if (node->left_child != nullptr)
    {
        erase(node->left_child);
    }
    if (node->right_child != nullptr)
    {
        erase(node->right_child);
    }
    delete node;
}
