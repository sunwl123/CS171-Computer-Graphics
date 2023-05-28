#include "utils.h"
#include "scene.h"

bool ObjectKDTreeNode::if_leaf(Face *face)
{
    // judge if the node is a leaf
    Vec3f faceMin = face->min();
    Vec3f faceMax = face->max();
    return (faceMin.x() < max.x() || faceMin.x() == max.x() && faceMin.x() == faceMax.x()) && (faceMax.x() > min.x() || faceMax.x() == min.x() && faceMin.x() == faceMax.x()) && (faceMin.y() < max.y() || faceMin.y() == max.y() && faceMin.y() == faceMax.y()) && (faceMax.y() > min.y() || faceMax.y() == min.y() && faceMin.y() == faceMax.y()) && (faceMin.z() < max.z() || faceMin.z() == max.z() && faceMin.z() == faceMax.z()) && (faceMax.z() > min.z() || faceMax.z() == min.z() && faceMin.z() == faceMax.z());
}

ObjectKDTreeNode *ObjectKDTree::build(int depth, int d, vector<Face *> *faces, Vec3f min, Vec3f max)
{
    // build the kd-tree
    ObjectKDTreeNode *p = new ObjectKDTreeNode;
    p->min = min;
    p->max = max;
    Vec3f maxl, minr;
    if (d == 0)
    {
        maxl = Vec3f((p->max.x() + p->min.x()) / 2, p->max.y(), p->max.z());
        minr = Vec3f((p->max.x() + p->min.x()) / 2, p->min.y(), p->min.z());
    }
    else if (d == 1)
    {
        maxl = Vec3f(p->max.x(), (p->max.y() + p->min.y()) / 2, p->max.z());
        minr = Vec3f(p->min.x(), (p->max.y() + p->min.y()) / 2, p->min.z());
    }
    else
    {
        maxl = Vec3f(p->max.x(), p->max.y(), (p->max.z() + p->min.z()) / 2);
        minr = Vec3f(p->min.x(), p->min.y(), (p->max.z() + p->min.z()) / 2);
    }
    p->faces = new vector<Face *>;
    for (auto face : *faces)
    {
        if (p->if_leaf(face))
        {
            p->faces->push_back(face);
        }
    }
    const int max_faces = 8;
    const int max_depth = 24;
    if (p->faces->size() > max_faces && depth < max_depth)
    {
        p->left_child = build(depth + 1, (d + 1) % 3, p->faces, min, maxl);
        p->right_child = build(depth + 1, (d + 1) % 3, p->faces, minr, max);
        vector<Face *> *left_faces = p->left_child->faces;
        vector<Face *> *right_faces = p->right_child->faces;
        std::map<Face *, int> cnt;
        for (auto face : *left_faces)
        {
            cnt[face]++;
        }
        for (auto face : *right_faces)
        {
            cnt[face]++;
        }
        p->left_child->faces = new vector<Face *>;
        p->right_child->faces = new vector<Face *>;
        p->faces->clear();
        for (auto face : *left_faces)
        {
            if (cnt[face] == 1)
            {
                p->left_child->faces->push_back(face);
            }
            else
            {
                p->faces->push_back(face);
            }
        }
        for (auto face : *right_faces)
        {
            if (cnt[face] == 1)
            {
                p->right_child->faces->push_back(face);
            }
        }
    }
    else
    {
        p->left_child = nullptr;
        p->right_child = nullptr;
    }
    return p;
}

void ObjectKDTree::get_faces(ObjectKDTreeNode *node, vector<Face *> *faces)
{
    // get the faces in the kd-tree node
    node->left = faces->size();
    for (auto face : *node->faces)
    {
        faces->push_back(face);
    }
    node->right = faces->size();
    if (node->left_child != nullptr)
    {
        get_faces(node->left_child, faces);
    }
    if (node->right_child != nullptr)
    {
        get_faces(node->right_child, faces);
    }
}

ObjectKDTree::ObjectKDTree(vector<Face *> *faces)
{
    // init the tree root
    Vec3f min = Vec3f(1e10, 1e10, 1e10);
    Vec3f max = Vec3f(-1e10, -1e10, -1e10);
    for (auto face : *faces)
    {
        min = minVec3f(min, face->min());
        max = maxVec3f(max, face->max());
    }
    root = build(1, 0, faces, min, max);
    this->faces = new vector<Face *>;
    get_faces(root, this->faces);
}

double ObjectKDTree::intersect_node(ObjectKDTreeNode *node, Ray ray)
{
    // intersect the kd-tree node
    if (!(ray.point_s >= node->min && ray.point_s <= node->max))
    {
        float t = -1e10;
        if (fabs(ray.vector_d.x()) > 0)
        {
            t = std::max(t, std::min((node->min.x() - ray.point_s.x()) / ray.vector_d.x(), (node->max.x() - ray.point_s.x()) / ray.vector_d.x()));
        }
        if (fabs(ray.vector_d.y()) > 0)
        {
            t = std::max(t, std::min((node->min.y() - ray.point_s.y()) / ray.vector_d.y(), (node->max.y() - ray.point_s.y()) / ray.vector_d.y()));
        }
        if (fabs(ray.vector_d.z()) > 0)
        {
            t = std::max(t, std::min((node->min.z() - ray.point_s.z()) / ray.vector_d.z(), (node->max.z() - ray.point_s.z()) / ray.vector_d.z()));
        }
        if (t < -(1e-6))
        {
            return 1e10;
        }
        Vec3f pp = ray.point_s + ray.vector_d * t;
        if (!(pp >= node->min && pp <= node->max))
        {
            return 1e10;
        }
        return t;
    }
    else
    {
        return -1e10;
    }
}

void ObjectKDTree::intersect_faces(ObjectKDTreeNode *node, Ray ray, Face *&face, double &tmin, Vec3f &normal)
{
    // intersect the faces in the kd-tree node
    for (int i = 0; i < node->faces->size(); ++i)
    {
        std::pair<double, Vec3f> r = (*node->faces)[i]->intersect(ray);
        double t = r.first;
        if (t > 0 && t < tmin)
        {
            tmin = t;
            // if ((*node->faces)[i] == nullptr)
            //     printf("intersect_faces::nullptr\n");
            // printf("intersect_faces::faces = %x\n", (*node->faces)[i]);
            face = (*node->faces)[i];
            normal = r.second;
        }
    }
    // if (face != nullptr)
    //     printf("face = %x\n", face);
    double tleft = node->left_child ? intersect_node(node->left_child, ray) : 1e10;
    double tright = node->right_child ? intersect_node(node->right_child, ray) : 1e10;
    if (tleft < tright)
    {
        if (tmin <= tleft)
        {
            return;
        }
        if (node->left_child)
        {
            intersect_faces(node->left_child, ray, face, tmin, normal);
        }
        if (tmin <= tright)
        {
            return;
        }
        if (node->right_child)
        {
            intersect_faces(node->right_child, ray, face, tmin, normal);
        }
    }
    else
    {
        if (tmin <= tright)
        {
            return;
        }
        if (node->right_child)
        {
            intersect_faces(node->right_child, ray, face, tmin, normal);
        }
        if (tmin <= tleft)
        {
            return;
        }
        if (node->left_child)
        {
            intersect_faces(node->left_child, ray, face, tmin, normal);
        }
    }
}

void Scene::add_object(Object *object)
{
    // add the object to the scene
    for (int i = 0; i < object->faces_count; ++i)
    {
        object->faces[i]->object = object;
    }
    objects.push_back(object);
}

Ray Scene::generate_ray(long long step)
{
    // generate the ray
    double alpha = Utils::my_random(0, 2 * PI, 0, step);
    Vec3f s = Points + Vec3f(cos(alpha), 0, sin(alpha)) * radius;
    Vec3f d = sampleRay(Normals, 0, step);
    return Ray(s + d * 1e-6, d);
}

void Scene::ray_trace(const Ray &ray, const Vec3f &color_weight, int depth, long long step, HitPoint *hitpoint)
{
    // ray trace
    if (depth > 10)
    {
        return;
    }
    double tmin = 1e10;
    Face *next_face = nullptr;
    Vec3f normal;
    objectKDTree->intersect_faces(objectKDTree->root, ray, next_face, tmin, normal);

    // if (next_face != nullptr)
    //     printf("trace::face = %x\n", next_face, tmin);
    // printf("trace::face = %x, tmin = %f\n", next_face, tmin);
    if (next_face == nullptr)
    {
        return;
    }
    // printf("scene::trace::if::faq\n");
    if (!hitpoint && tmin < 1e-3)
        return;
    Vec3f p = ray.point_s + ray.vector_d * tmin;
    double s = brdf_settings[next_face->BRDF].specular + brdf_settings[next_face->BRDF].diffuse + brdf_settings[next_face->BRDF].refraction; // s = 1
    double action = Utils::my_random(0, 1) * s;
    Vec3f dr = ray.vector_d - normal * (2 * ray.vector_d.dot(normal));
    if (brdf_settings[next_face->BRDF].specular > 0 && action <= brdf_settings[next_face->BRDF].specular)
    {
        ray_trace(Ray(p + dr * 1e-6, dr), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
        return;
    }
    action -= brdf_settings[next_face->BRDF].specular;
    if (brdf_settings[next_face->BRDF].diffuse > 0 && action <= brdf_settings[next_face->BRDF].diffuse)
    {

        if (hitpoint)
        {
            hitpoint->position = p;
            // printf("trace::color = %f, %f, %f\n", next_face->texture->get_color(p).x(), next_face->texture->get_color(p).y(), next_face->texture->get_color(p).z());
            hitpoint->color_weight = color_weight.cwiseProduct(next_face->texture->get_color(p)) * s;
            // printf("trace::color = %f, %f, %f\n", hitpoint->color_weight.x(), hitpoint->color_weight.y(), hitpoint->color_weight.z());
            // hitpoint->color_flux_light = hitpoint->color_flux_light + hitpoint->color_weight;
            hitpoint->color_flux_light = hitpoint->color_flux_light + hitpoint->color_weight * (next_face->BRDF == LIGHT);
            // printf("trace::color = %f, %f, %f\n", hitpoint->color_flux_light.x(), hitpoint->color_flux_light.y(), hitpoint->color_flux_light.z());
            hitpoint->brdf_type = brdf_settings[next_face->BRDF];
            hitpoint->normal = normal;
            if (next_face->BRDF == LIGHT)
            {
                // hitpoint->color_flux_light = hitpoint->color_flux_light + hitpoint->color_weight;
                hitpoint->is_valid = false;
            }
            else
            {
                hitpoint->is_valid = true;
            }
        }
        else
        {
            double a = Utils::my_random();
            if (a <= brdf_settings[next_face->BRDF].s)
            {
                // printf("scene::trace::if::faq\n");
                Vec3f d = sampleRay(dr, depth, step, brdf_settings[next_face->BRDF].phone_lighting);
                ray_trace(Ray(p + d * 1e-6, d), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
            }
            else
            {
                // printf("scene::trace::else::faq\n");
                a -= brdf_settings[next_face->BRDF].s;
                hitKDTree->update(hitKDTree->root, p, color_weight, ray.vector_d);
                Vec3f d = sampleRay(normal, depth, step);
                if (d.dot(normal) < 0)
                {
                    d = -d;
                }
                if (a <= brdf_settings[next_face->BRDF].d)
                {
                    ray_trace(Ray(p + d * 1e-6, d), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
                }
            }
        }
        return;
    }
    action -= brdf_settings[next_face->BRDF].diffuse;
    if (brdf_settings[next_face->BRDF].refraction > 0 && action <= brdf_settings[next_face->BRDF].refraction)
    {
        if (!next_face->object->center)
        {
            next_face->object->calculate_center();
        }
        bool res = (*(next_face->object)->center - p).dot(normal) < 0;
        double refractive_index = brdf_settings[next_face->BRDF].refract_index;
        if (!res)
        {
            refractive_index = 1.0f / refractive_index;
        }
        double costhetaIn = -ray.vector_d.dot(normal);
        double costhetaOut2 = 1 - (1 - costhetaIn * costhetaIn) / (refractive_index * refractive_index);
        if (costhetaOut2 >= -(1e-6))
        {
            double costhetaOut = sqrtf(costhetaOut2);
            double r0 = ((1 - refractive_index) / (1 + refractive_index)) * ((1 - refractive_index) / (1 + refractive_index));
            double costheta = res ? costhetaIn : costhetaOut;
            double r = r0 + (1 - r0) * pow(1 - costheta, 5);
            if (Utils::my_random() <= r)
            {
                ray_trace(Ray(p + dr * 1e-6, dr), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
            }
            else
            {
                Vec3f d = ray.vector_d / refractive_index + normal * (costhetaIn / refractive_index - costhetaOut);
                ray_trace(Ray(p + d * 1e-6, d), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
            }
        }
        else
        {
            ray_trace(Ray(p + dr * 1e-6, dr), color_weight.cwiseProduct(next_face->texture->get_color(p)) * s, depth + 1, step, hitpoint);
        }
    }
}

Vec3f Scene::sampleRay(Vec3f normal, int depth, long long step, double s)
{
    // sample the ray
    Vec3f u = Vec3f(1, 0, 0).cross(normal);
    if (u.x() * u.x() + u.y() * u.y() + u.z() * u.z() < 1e-6)
    {
        u = Vec3f(0, 1, 0).cross(normal);
    }
    u.normalize();
    Vec3f v = normal.cross(u);
    v.normalize();
    double theta = Utils::my_random(0, 2 * PI, 2 * depth + 1, step);
    double phi = asin(pow(Utils::my_random(0, 1, 2 * depth + 2, step), 1.0f / (s + 1)));
    Vec3f res = (normal * cos(phi) + (u * cos(theta) + v * sin(theta)) * sin(phi)).normalized();
    return res;
}

void Scene::init_hitKDTree(vector<HitPoint *> *hitpoints)
{
    // init the hitpoint kd-tree
    if (hitKDTree)
    {
        delete hitKDTree;
    }
    hitKDTree = new KDTree(hitpoints);
    fprintf(stderr, "hitpoint kd-tree init finished\n");
}

void Scene::init_objectKDTree()
{
    // init the object kd-tree
    std::vector<Face *> *faces = new std::vector<Face *>;
    for (auto object : objects)
    {
        for (int i = 0; i < object->faces_count; ++i)
        {
            faces->push_back(object->faces[i]);
        }
    }
    objectKDTree = new ObjectKDTree(faces);
    fprintf(stderr, "object kd-tree init finished\n");
}