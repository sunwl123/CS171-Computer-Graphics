#ifndef _MESH_H_
#define _MESH_H_
#include "defines.h"
#include <vector>

typedef glm::vec3 vec3;
typedef glm::uvec3 uvec3;

struct Vertex
{
  vec3 position;
  vec3 normal;
};

class Mesh
{
public:
  /**
   * construct a mesh from a obj file
   */
  Mesh(const std::string &path);
  /**
   * draw mesh objects
   */
  void draw() const;
  glm::mat4 Model;

private:
  // mesh data
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  unsigned int bunny_VAO;

  void loadDataFromFile(const std::string &path);
};
#endif