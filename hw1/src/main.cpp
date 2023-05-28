#include <utils.h>
#include <mesh.h>
#include <shader.h>
#include <sstream>

const int WIDTH = 1920;
const int HEIGHT = 1080;

glm::vec3 initial(0, 0, 0);
glm::vec3 front(0, 0, 1);
glm::vec3 up(0, 1, 0);
glm::vec3 right(1, 0, 0);
glm::vec3 light_position = glm::vec3(0, 1, 0);
glm::vec3 cam_pos = glm::vec3(1, 1, 1);

float angle = 3.14f;
float vertice_angle = 0.0f;
float view = 45.0f;
float cam_speed = 1.0f;
float mouse_speed = 0.002f;

glm::vec3 cam_front(0, 0, -1);
glm::vec3 cam_right(-1, 0, 0);
glm::vec3 cam_up(0, 1, 0);

float delta = 0.0f;
float last_frame = 0.0f;
float current_frame = 0.0f;

bool first_load = true;
float prev_x = WIDTH / 2;
float prev_y = HEIGHT / 2;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow *window, double x, double y);

GLFWwindow *window;

struct Light
{
  float ambient;
  float specular;
  float diffuse;
  float angle;
  float low;
  glm::vec3 power;
  glm::vec3 direction;
  glm::vec3 position;
  Light()
  {
    ambient = 0.0f;
    specular = 5.0f;
    angle = 45.0f;
    low = 1.0f;
    power = glm::vec3(1.0f);
    direction = glm::vec3(0.0, 0.0, -10.0);
    position = glm::vec3(0.0, 0.0, 10.0);
  }
};

void set_float(const char *name, int index, const float &num, Shader *shader)
{
  std::ostringstream in;
  in << "all_Lights[" << index << "]." << name;
  std::string attribute = in.str();
  shader->setFloat(attribute.c_str(), num);
}

void set_vec3(const char *name, int index, const glm::vec3 &num, Shader *shader)
{
  std::ostringstream in;
  in << "all_Lights[" << index << "]." << name;
  std::string attribute = in.str();
  shader->setVec3(attribute.c_str(), num);
}

void light_init(std::vector<Light> lights, Shader *shader)
{
  for (int i = 0; i < 2; i++)
  {
    set_float("low", i, lights[i].low, shader);
    set_float("ambient", i, lights[i].ambient, shader);
    set_float("specular", i, lights[i].specular, shader);
    set_float("angle", i, lights[i].angle, shader);
    set_vec3("power", i, lights[i].power, shader);
    set_vec3("direction", i, lights[i].direction, shader);
    set_vec3("position", i, lights[i].position, shader);
  }
}

std::vector<Light> lights(2);

void set_light0()
{
  lights[0].position = glm::vec3(0, 1, 0);
  lights[0].power = glm::vec3(2, 2, 2);
  lights[0].low = 0.1f;
  lights[0].ambient = 0.0f;
  lights[0].angle = 45.0f;
  lights[0].direction = glm::vec3(0, -1, 0);
}

void set_light1()
{
  lights[1].power = glm::vec3(0, 0, 1);
  lights[1].low = 0.5f;
  lights[1].ambient = 0.0f;
  lights[1].angle = 10.0f;
  lights[1].position = cam_pos;
  lights[1].direction = cam_front;
}

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CS171 Homework 1", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glEnable(GL_DEPTH_TEST);

  Mesh bunny("../../assets/bunny.obj");
  /*for(auto iter = bunny.vertices.begin();iter!=bunny.vertices.end();iter++)
  {
    std::cout<<iter->position.x<<" "<<iter->position.y<<" "<<iter->position.z<<std::endl;
  }*/
  /*for(int i=0;i<bunny.indices.size();i++)
  {
    std::cout<<bunny.indices[i]<<" ";
  }*/
  glm::mat4 Model(1.0f);
  bunny.Model = Model;
  Shader shader("../../src/vertex_shader.vs",
                "../../src/frag_shader.fs",
                "../../src/geometry_shader.gs");

  set_light0();
  set_light1();

  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSetCursorPosCallback(window, mouse_callback);
    current_frame = glfwGetTime();
    delta = current_frame - last_frame;
    last_frame = current_frame;
    processInput(window);
    glfwSetScrollCallback(window, scroll_callback);

    lights[1].position = cam_pos;
    lights[1].direction = cam_front;
    light_init(lights, &shader);

    glm::mat4 Projection;
    glm::mat4 View;

    Projection = glm::perspective(glm::radians(view), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    View = glm::lookAt(cam_pos, cam_pos + cam_front, up);

    shader.setVec3("cam_pos", cam_pos);
    shader.setMat4("Projection", Projection);
    shader.setMat4("View", View);
    shader.setMat4("Model", Model);
    shader.use();
    bunny.draw();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam_pos += cam_speed * delta * cam_front;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam_pos -= cam_speed * delta * cam_front;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam_pos -= cam_speed * delta * cam_right;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam_pos += cam_speed * delta * cam_right;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cam_pos += cam_speed * delta * cam_up;
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    cam_pos -= cam_speed * delta * cam_up;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
  if (first_load == true)
  {
    prev_x = x_pos;
    prev_y = y_pos;
    first_load = false;
  }
  angle += (prev_x - x_pos) * mouse_speed;
  vertice_angle += (prev_y - y_pos) * mouse_speed;
  prev_x = x_pos;
  prev_y = y_pos;
  if (vertice_angle > 89.0f)
  {
    vertice_angle = 89.0f;
  }
  if (vertice_angle < -89.0f)
  {
    vertice_angle = -89.0f;
  }
  cam_front = glm::vec3(cos(vertice_angle) * sin(angle), sin(vertice_angle), cos(vertice_angle) * cos(angle));
  cam_right = glm::normalize(glm::cross(cam_front, up));
  cam_up = glm::normalize(glm::cross(cam_right, cam_front));
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  if(view>=1.0f && view<=45.0f)
  {
    view-=yoffset;
  }
  if (view <= 1.0f)
  {
    view = 1.0f;
  }
  if (view >= 45.0f)
  {
    view = 45.0f;
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}
