#include "object.h"
#include "utils.h"

void Object::import_ply(char *filename, TextureMap *texture, int BRDF)
{
    // import the ply file
    FILE *file = fopen(filename, "r");
    char buffer[1024];
    faces_count = 0;
    vertexes_count = 0;
    while (fgets(buffer, 1024, file))
    {
        if (string(buffer) == "end_header\n")
        {
            break;
        }
        vector<string> tokens = Utils::split(buffer);
        if (tokens[0] == "element")
        {
            if (tokens[1] == "vertex")
            {
                vertexes_count = stoi(tokens[2]);
            }
            else if (tokens[1] == "face")
            {
                faces_count = stoi(tokens[2]);
            }
        }
    }
    vertexes = new Vec3f *[vertexes_count];
    for (int i = 0; i < vertexes_count; ++i)
    {
        double x, y, z;
        fscanf(file, "%lf%lf%lf", &x, &y, &z);
        fgets(buffer, 1024, file);
        vertexes[i] = new Vec3f(x, y, z);
    }
    faces = new Face *[faces_count];
    for (int i = 0; i < faces_count; ++i)
    {
        int n;
        fscanf(file, "%d", &n);
        if (n != 3)
        {
            throw runtime_error("Only triangles are supported");
        }
        int a, b, c;
        fscanf(file, "%d%d%d", &a, &b, &c);
        faces[i] = new Triangle(vertexes[a], vertexes[b], vertexes[c], texture, BRDF);
    }
    fclose(file);
    fprintf(stderr, "Imported %d faces and %d vertexes from %s\n", faces_count, vertexes_count, filename);
}

// void Object::import_bpt(char *filename, TextureMap *texture, int BRDF)
// {
//     // import the bpt file
//     FILE *file = fopen(filename, "r");
//     fscanf(file, "%d", &faces_count);
//     faces = new Face *[faces_count];
//     vector<Vec3f *> ver;
//     for (int i = 0; i < faces_count; ++i)
//     {
//         int n, m;
//         fscanf(file, "%d%d", &n, &m);
//         Vec3f **p = new Vec3f *[n + 1];
//         for (int j = 0; j <= n; ++j)
//         {
//             p[j] = new Vec3f[m + 1];
//             for (int k = 0; k <= m; ++k)
//             {
//                 fscanf(file, "%lf%lf%lf", &p[j][k].x(), &p[j][k].y(), &p[j][k].z());
//                 ver.push_back(&p[j][k]);
//             }
//         }
//         faces[i] = new Bezier(n, m, p, texture, BRDF);
//     }
//     vertexes_count = ver.size();
//     vertexes = new Vec3f *[vertexes_count];
//     for (int i = 0; i < vertexes_count; ++i)
//     {
//         vertexes[i] = ver[i];
//     }
//     fclose(file);
//     fprintf(stderr, "Imported %d faces from %s\n", faces_count, filename);
// }

void Object::calculate_center()
{
    // calculate the object center
    center = new Vec3f(0, 0, 0);
    for (int i = 0; i < vertexes_count; ++i)
    {
        *center += *vertexes[i];
    }
    *center /= vertexes_count;
}

void Object::print_box()
{
    Vec3f min(1e10, 1e10, 1e10);
    Vec3f max(-1e10, -1e10, -1e10);
    for (int i = 0; i < faces_count; ++i)
    {
        min = minVec3f(min, faces[i]->min());
        max = maxVec3f(max, faces[i]->max());
    }
    printf("min::%.5f %.5f %.5f\n", min.x(), min.y(), min.z());
    printf("max::%.5f %.5f %.5f\n", max.x(), max.y(), max.z());
}

void Object::scale(
    double x1, double x2, double x3, double x4,
    double y1, double y2, double y3, double y4,
    double z1, double z2, double z3, double z4)
{
    // scale the object
    for (int i = 0; i < vertexes_count; ++i)
    {
        Vec3f ver = *vertexes[i];
        vertexes[i]->x() = x1 * ver.x() + x2 * ver.y() + x3 * ver.z() + x4;
        vertexes[i]->y() = y1 * ver.x() + y2 * ver.y() + y3 * ver.z() + y4;
        vertexes[i]->z() = z1 * ver.x() + z2 * ver.y() + z3 * ver.z() + z4;
    }
    for (int i = 0; i < faces_count; ++i)
    {
        faces[i]->scale(x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4);
    }
    calculate_center();
}

void Object::rotate(double angle)
{
    // rotate the object
    calculate_center();
    for (int i = 0; i < vertexes_count; ++i)
    {
        Vec3f dis = *vertexes[i] - *center;
        *vertexes[i] = *center + Vec3f(cos(angle) * dis.x() - sin(angle) * dis.z(), dis.y(), sin(angle) * dis.x() + cos(angle) * dis.z());
    }
}

Vec3f Triangle::min()
{
    // get the triangle min
    return minVec3f(*p1, minVec3f(*p2, *p3));
}

Vec3f Triangle::max()
{
    // get the triangle max
    return maxVec3f(*p1, maxVec3f(*p2, *p3));
}

Vec3f Triangle::center()
{
    // get the triangle center
    return (*p1 + *p2 + *p3) / 3;
}

pair<double, Vec3f> Triangle::intersect(Ray ray)
{
    // intersect the triangle
    Vec3f e1 = *p1 - *p2;
    Vec3f e2 = *p1 - *p3;
    Vec3f s1 = *p1 - ray.point_s;
    Mat3f A;
    A.row(0) = s1;
    A.row(1) = e1;
    A.row(2) = e2;
    double t = A.determinant();
    Mat3f B;
    B.row(0) = ray.vector_d;
    B.row(1) = s1;
    B.row(2) = e2;
    double beta = B.determinant();
    Mat3f C;
    C.row(0) = ray.vector_d;
    C.row(1) = e1;
    C.row(2) = s1;
    double gamma = C.determinant();
    Mat3f D;
    D.row(0) = ray.vector_d;
    D.row(1) = e1;
    D.row(2) = e2;
    double n = D.determinant();
    t /= n;
    beta /= n;
    gamma /= n;
    if (!(beta >= -(1e-6) && beta <= 1 + 1e-6 && gamma >= -(1e-6) && gamma <= 1 + 1e-6 && beta + gamma <= 1 + 1e-6))
    {
        t = -1;
    }
    Vec3f norm = (*p2 - *p1).cross(*p3 - *p1);
    if (norm.dot(ray.vector_d) > 0)
    {
        norm = -norm;
    }
    norm.normalize();
    return make_pair(t, norm);
}

double Triangle::intersect_plane(Ray ray)
{
    // intersect the triangle plane
    Vec3f e1 = *p1 - *p2;
    Vec3f e2 = *p1 - *p3;
    Vec3f s1 = *p1 - ray.point_s;
    Mat3f A;
    A.row(0) = s1;
    A.row(1) = e1;
    A.row(2) = e2;
    double t = A.determinant();
    Mat3f D;
    D.row(0) = ray.vector_d;
    D.row(1) = e1;
    D.row(2) = e2;
    double n = D.determinant();
    t /= n;
    return t;
}

Vec3f SphereF::min()
{
    // get the sphere min
    return point - Vec3f(radius, radius, radius);
}

Vec3f SphereF::max()
{
    // get the sphere max
    return point + Vec3f(radius, radius, radius);
}

Vec3f SphereF::center()
{
    // get the sphere center
    return point;
}

pair<double, Vec3f> SphereF::intersect(Ray ray)
{
    // intersect the sphere
    Vec3f dis = point - ray.point_s;
    double dis2 = dis.x() * dis.x() + dis.y() * dis.y() + dis.z() * dis.z();
    int pos = 0;
    if (dis2 > radius * radius)
    {
        pos = 1;
    }
    else if (dis2 < radius * radius)
    {
        pos = -1;
    }
    double tp = dis.dot(ray.vector_d);
    if (pos > 0 && tp < 0)
    {
        return make_pair(-1, Vec3f(0.0f, 0.0f, 0.0f));
    }
    double d2 = dis2 - tp * tp;
    if (d2 > radius * radius || pos == 0)
    {
        return make_pair(-1, Vec3f(0.0f, 0.0f, 0.0f));
    }
    double t;
    if (pos > 0)
    {
        t = tp - sqrt(radius * radius - d2);
    }
    else if (pos < 0)
    {
        t = tp + sqrt(radius * radius - d2);
    }
    Vec3f norm = ray.point_s + ray.vector_d * t - point;
    if (norm.dot(ray.vector_d) > 0)
    {
        norm = -norm;
    }
    norm.normalize();
    return make_pair(t, norm);
}

Vec3f Disc::min()
{
    // get the disc min
    return point - Vec3f(radius, 0, radius);
}

Vec3f Disc::max()
{
    // get the disc max
    return point + Vec3f(radius, 0, radius);
}

Vec3f Disc::center()
{
    // get the disc center
    return point;
}

pair<double, Vec3f> Disc::intersect(Ray ray)
{
    // intersect the disc
    double t = 0;
    if (fabs(ray.vector_d.y()) < 1e-6 && fabs(ray.point_s.y() - point.y()) > 1e-6)
    {
        return make_pair(-1, Vec3f(0.0f, 0.0f, 0.0f));
    }
    else
    {
        t = (point.y() - ray.point_s.y()) / ray.vector_d.y();
    }
    Vec3f p = ray.point_s + ray.vector_d * t;
    if ((p.x() - point.x()) * (p.x() - point.x()) + (p.z() - point.z()) * (p.z() - point.z()) <= radius * radius)
    {
        return make_pair(t, Vec3f(0.0f, -1.0f, 0.0f));
    }
    else
    {
        return make_pair(-1, Vec3f(0.0f, 0.0f, 0.0f));
    }
}

Sphere::Sphere(Vec3f c, double radius, TextureMap *texture, int BRDF)
{
    // init the sphere
    vertexes_count = 0;
    faces_count = 1;
    faces = new Face *[faces_count]
    {
        new SphereF(c, radius, texture, BRDF)
    };
    center = new Vec3f(c);
}