#include "material.h"
#include <iostream>
#include <utility>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
    : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
    : diffuse_color(std::move(diff)),
      specular_color(std::move(spec)),
      ambient_color(std::move(ambient)),
      shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
    InteractionPhongLightingModel m;
    // TODO: Your code here.
    m.ambient = ambient_color;
    m.diffusion = diffuse_color;
    m.shininess = shininess;
    m.specular = specular_color;
    return m;
}

TextureMat::TextureMat()
{
    tex_diff = new Texture();
    tex_disp = new Texture();
    tex_norm = new Texture();
}

void TextureMat::set_diff_map(const std::string &diff_path)
{
    tex_diff->loadTextureFromFile(diff_path);
    tex_diff->type = Texture::DIFF;
}

void TextureMat::set_disp_map(const std::string &disp_path)
{
    tex_disp->loadTextureFromFile(disp_path);
    tex_disp->type = Texture::DISP;
}

void TextureMat::set_norm_map(const std::string &norm_path)
{
    tex_norm->loadTextureFromFile(norm_path);
    tex_norm->type = Texture::NORM;
}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
    InteractionPhongLightingModel m;
    Vec3f res;
    res = tex_disp->interpolation(interaction.uv.x(), interaction.uv.y());
    //std::cout<<"faq"<<std::endl;
    m.ambient = res;
    m.diffusion = res;
    m.specular = res;
    m.shininess = 16.f;
    return m;
}

