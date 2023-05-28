#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

class Texture {
 public:
    enum Type { NONE, DIFF, NORM, DISP };
    Type type;
    Texture();
    void loadTextureFromFile(const std::string &path);
    Vec3f get_data(int index);
    Vec3f interpolation(float u, float v);
    int w, h, ch;
 private:
    std::vector<uint8_t> tex_data;
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
