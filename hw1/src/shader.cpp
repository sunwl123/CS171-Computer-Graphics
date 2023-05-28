#include <shader.h>
#include <utils.h>
#include <fstream>
#include <sstream>
#include <vector>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath)
{
  init(vsPath, fsPath, gsPath);
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath)
{
  initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
}

void Shader::initWithCode(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath)
{
  // TODO: filling this function to set shaders to OpenGL
  GLint False = GL_FALSE;
  int Info_Log_Length;
  const char *vertex_shader = vsPath.c_str();
  const char *fragment_shader = fsPath.c_str();
  const char *geometry_shader = gsPath.c_str();
  GLuint vertex, fragment, geometry;
  char infoLog[512];
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertex_shader, NULL);
  glCompileShader(vertex);
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragment_shader, NULL);
  glCompileShader(fragment);
  geometry = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometry, 1, &geometry_shader, NULL);
  glCompileShader(geometry);
  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glAttachShader(id, geometry);
  glLinkProgram(id);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  glDeleteShader(geometry);
}

std::string Shader::getCodeFromFile(const std::string &path)
{
  std::string code;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    file.open(getPath(path));
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  }
  catch (std::ifstream::failure &e)
  {
    LOG_ERR("File Error: " + std::string(e.what()));
  }
  return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const
{
  return glGetUniformLocation(id, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const
{
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const
{
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const
{
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const
{
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const
{
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const
{
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
