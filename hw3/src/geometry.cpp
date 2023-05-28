#include "geometry.h"

#include <utility>
#include <iostream>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.
    //std::cout<<"faq tri1"<<std::endl;
    Vec3f v0_v1,v0_v2,v0_p,s1,s2;
    v0_v1 = v1 - v0;
    v0_v2 = v2 - v0;
    v0_p = ray.origin - v0;
    s1 = ray.direction.cross(v0_v2);
    s2 = v0_p.cross(v0_v1);
    float t = s2.dot(v0_v2) / s1.dot(v0_v1);
    float check1 = s1.dot(v0_p) / s1.dot(v0_v1);
    float check2 = s2.dot(ray.direction) / s1.dot(v0_v1);
    if (t > 0.0f && check1 > 0.0f && check2 > 0.0f && (1.0f - check1 - check2) > 0.0f){
        if (t < interaction.dist) {
            interaction.dist = t;
            interaction.pos = ray.origin + t * ray.direction;
            interaction.type = Interaction::Type::GEOMETRY;
            interaction.normal = normal;
            interaction.uv = Vec2f(check1, check2);
            interaction.model = material->evaluate(interaction);
            //std::cout<<"faq tri2"<<std::endl;
            return true;
        }
    }
    return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {} //normalized?

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here
    Vec3f n = normal.normalized();
    float t = (position - ray.origin).dot(n) / (ray.direction.dot(n));
    Vec3f P = ray.origin + t * ray.direction;
    Vec3f P0_P = P - position;
    float pixel_x,pixel_y;
    Vec3f tang,cotg;
    tang = tangent.normalized();
    cotg = n.cross(tang).normalized();
    pixel_x = P0_P.dot(tang);
    pixel_y = P0_P.dot(cotg);

    ConstColorMat color_mat; //for if using

    if (std::abs(pixel_x) <= size.x()/2 && std::abs(pixel_y) <= size.y()/2 && t > 0.0f) {
        if (t < interaction.dist) {
            interaction.dist = t;
            interaction.pos = P;
            interaction.type = Interaction::Type::GEOMETRY;
            interaction.uv = Vec2f(pixel_x/(2*size.x())+0.5f, pixel_y/(2*size.y())+0.5f);
            if(this->material != nullptr){
                if(this->material->tex_norm != nullptr){
                    //std::cout<<"uv = "<<interaction.uv<<std::endl;
                    //std::cout<<"w = "<<material->tex_norm->w<<std::endl;
                    //std::cout<<"h = "<<material->tex_norm->h<<std::endl;
                    Vec2f pos;
                    Vec3f local_normal, true_normal;
                    pos = Vec2f(interaction.uv.x(), interaction.uv.y()).cwiseProduct(Vec2f((float)material->tex_norm->w, (float)material->tex_norm->h));
                    //std::cout<<"faq_rec1"<<std::endl;
                    //std::cout<<"pos_x = "<<(int)pos.x()<<std::endl;
                    //std::cout<<"pos_y = "<<(int)pos.y()<<std::endl;
                    //std::cout<<"h = "<<material->tex_norm->h<<std::endl;
                    local_normal = material->tex_norm->get_data((int)pos.x() * material->tex_norm->h + (int)pos.y());
                    //std::cout<<"faq_rec2"<<std::endl;
                    true_normal = local_normal.x() * tang + local_normal.y() * cotg + local_normal.z() * n;
                    interaction.normal = true_normal.normalized();
                    // std::printf("%f %f %f\n", interaction.normal(0), interaction.normal(1), interaction.normal(2));
                    //interaction.normal = n;
                }
                else
                {
                    interaction.normal = n;
                }
                interaction.normal = n;
                interaction.model = this->material->evaluate(interaction);
            }
            else{
                interaction.normal = n;
                interaction.model = color_mat.evaluate(interaction);
            }
            return true;
        }
    }
    return false;
}
Vec2f Rectangle::getSize() const {
  return size;
}
Vec3f Rectangle::getNormal() const {
  return normal;
}
Vec3f Rectangle::getTangent() const {
  return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
    : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.
    //std::cout<<"faq ell1"<<std::endl;
    Eigen::Matrix4f S, R, T;
    T << 1.0f, 0.0f, 0.0f, p.x(),
         0.0f, 1.0f, 0.0f, p.y(),
         0.0f, 0.0f, 1.0f, p.z(),
         0.0f, 0.0f, 0.0f, 1.0f;
    S << a.norm(), 0.0f, 0.0f, 0.0f,
         0.0f, b.norm(), 0.0f, 0.0f,
         0.0f, 0.0f, c.norm(), 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f;
    Vec3f a_norm, b_norm, c_norm;
    a_norm = a.normalized();
    b_norm = b.normalized();
    c_norm = c.normalized();
    R << a_norm.x(), b_norm.x(), c_norm.x(), 0.0f,
         a_norm.y(), b_norm.y(), c_norm.y(), 0.0f,
         a_norm.z(), b_norm.z(), c_norm.z(), 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f;
    
    Eigen::Matrix4f M = T*R*S;
    Eigen::Matrix4f M_inverse;
    M_inverse = M.inverse();

    Vec4f o_4d;
    o_4d = M_inverse * Vec4f(ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.0f);
    Vec3f o;
    o = Vec3f(o_4d.x(), o_4d.y(), o_4d.z());
    Vec4f d_4d;
    d_4d = M_inverse * Vec4f(ray.direction.x(), ray.direction.y(), ray.direction.z(), 0.0f);
    Vec3f d;
    d = Vec3f(d_4d.x(), d_4d.y(), d_4d.z());
    float d_norm = d.norm();
    d /= d_norm;

    Vec3f L;
    L = Vec3f(0.0f, 0.0f, 0.0f) - o;
    float tca;
    tca = L.dot(d);
    if (tca > 0.0f) {
        float d2;
        d2 = L.dot(L) - tca*tca;
        float r2;
        r2 = 1.0f;
        if (d2 <= r2) {
            float tnc = std::sqrt(r2 - d2);
            float t;
            t = tca - tnc;
            if (t/d_norm < interaction.dist)
            {
                interaction.dist = t/d_norm;
                interaction.pos = ray.origin + t/d_norm * ray.direction;
                interaction.type = Interaction::Type::GEOMETRY;
                Vec3f n_sphere = o+t*d;
                Vec4f n_4d = M.inverse().transpose()*Vec4f(n_sphere.x(), n_sphere.y(), n_sphere.z(), 0.0f);
                interaction.normal = Vec3f(n_4d.x(), n_4d.y(), n_4d.z());
                interaction.model = material->evaluate(interaction);
                //std::cout<<"faq ell2"<<std::endl;
                return true;
            }   
        }
    }
    return false;
}
