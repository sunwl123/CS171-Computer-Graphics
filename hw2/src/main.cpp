#include <utils.h>
#include <camera.h>
#include "enum.h"
#include <object.h>
#include <shader.h>
#include <bezier.h>
#include <stdio.h>
#include <sstream>
#include "nurbs.hpp"

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double x, double y);

const int WIDTH = 800;
const int HEIGHT = 600;

bool firstMouse = true;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

GLFWwindow *window;
Camera mycamera;

BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface)
//            adaptive_single_bezier_surface,
//            adaptive_multi_bezier_surface,
//            single_spline_surface
/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */


RenderCase choice = RenderCase::_values()[0];

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
    specular = 8.0f;
    angle = 90.0f;
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
  for (int i = 0; i < 1; i++)
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

int main() {
    for (size_t index = 0; index < RenderCase::_size(); ++index) {
        RenderCase render_case = RenderCase::_values()[index];
        std::cout << index << ". " << +render_case << "\n";
    }
    while (true) {
        std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << "\n";
        std::string input;
        std::cin >> input;
        if (isNumber(input) &&
            std::stoi(input) >= 0 &&
            std::stoi(input) < RenderCase::_size()) {
            choice = RenderCase::_values()[std::stoi(input)];
            break;
        } else {
            std::cout << "Wrong input.\n";
        }
    }
    /**
     * Choose a rendering case in the terminal.
     * */
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    switch (choice) {
        case RenderCase::uniform_single_bezier_surface: {
            std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
            break;
        }
        case RenderCase::uniform_multi_bezier_surface: {
            std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
            break;
        }
    }

    std::vector<Object> objects;
    std::vector<NurbsSurface> faces;
    //std::vector<BezierSurface> faces;
    //faces=read("C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\assets\\tea.bzs");
    faces=read_nurbs("C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\assets\\tea.bzs");
    //faces=read_nurbs("C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\assets\\ducky.bzs");
    //faces=read("C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\assets\\ducky.bzs");
    for(auto face:faces)
    {
        Object temp_obj;
        temp_obj=face.generateObject();
        temp_obj.init();
        objects.push_back(temp_obj);
    }

    Shader shader("C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\src\\vertex_shader.vs",
                  "C:\\Users\\Administrator\\Desktop\\assignment-2-sunwl123\\src\\fragment_shader.fs");
    std::vector<Light> lights(1);
    lights[0].power=glm::vec3(2.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glm::mat4 Projection;
        glm::mat4 View;
        glm::mat4 Model;
        Projection=glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        View=mycamera.getViewMatrix();
        Model=glm::mat4(1.0f);
        lights[0].position=mycamera.Position;
        lights[0].direction=mycamera.Front;
        shader.setVec3("cam_pos", mycamera.Position);
        shader.setMat4("Projection", Projection);
        shader.setMat4("View", View);
        shader.setMat4("Model", Model);
        light_init(lights, &shader);
        for(auto obj:objects)
        {
            obj.drawElements(shader);
        }
        GLdouble equn[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        //glClipPlane(GL_CLIP_PLANE0, equn);
        //glEnable(GL_CLIP_PLANE0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mycamera.processWalkAround(Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mycamera.processWalkAround(Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mycamera.processWalkAround(Leftward);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mycamera.processWalkAround(Rightward);
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        mycamera.processWalkAround(Upward);
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        mycamera.processWalkAround(Downward);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        std::cout << "Camera position: (" << mycamera.Position.x << ", "
                  << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
    }
}

void mouse_callback(GLFWwindow *window, double x, double y) {
    x = (float) x;
    y = (float) y;
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    mycamera.processLookAround(x - lastX, y - lastY);

    // update record
    lastX = x;
    lastY = y;
}