#include <string>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "context.hpp"
#include "mesh.hpp"
#include "skybox.hpp"
#include "program.hpp"
#include "shadow.hpp"
#include <cmath>
#include <vector>
#include <random>

int main() {
  if (glfwInit() == GLFW_TRUE) {
  
    const int window_width = 1920;
    const int window_height = 1080;
    std::string window_title = "OpenGL Showcase";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (glewInit() == GLEW_OK) {
      context window_context = {
        .camera = camera(glm::radians(58.f), window_width, window_height, .1f, 512.f, glm::vec3(0.f, 1.f, 0.f)),
        .last_mouse_x = 0,
        .last_mouse_y = 0,
        .ambient_light = ambient_light(color_white, .1f),
        .point_lights = {
          point_light{
            .color = glm::vec3(1.f, 1.f, .7f),
            .position = glm::vec3(0.f),
            .diffusive = 1.f,
            .specular = 1.f,
            .constant = 1.f,
            .linear = .01f,
            .quadratic = 0.f
          },
          point_light{
            .color = glm::vec3(.5f, .5f, 1.f),
            .position = glm::vec3(0.f),
            .diffusive = .8f,
            .specular = .8f,
            .constant = 1.f,
            .linear = .05f,
            .quadratic = 0.f
          }
        },
        .viewport_width = window_width,
        .viewport_height = window_height
      };

      glClearColor(1.f, 1.f, 1.f, 1.f);

      int frame_buffer_width = 0;
      int frame_buffer_height = 0;
      glfwGetFramebufferSize(window, &frame_buffer_width, &frame_buffer_height);

      glViewport(0, 0, frame_buffer_width, frame_buffer_height);

      glfwSetWindowUserPointer(window, &window_context);

      glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        context* window_context = (context*)glfwGetWindowUserPointer(window);
        window_context->viewport_width = width;
        window_context->viewport_height = height;
      });

      glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        context* window_context = (context*)glfwGetWindowUserPointer(window);
        glm::vec3 position = window_context->camera.position();
        glm::vec3 front = window_context->camera.front();
        glm::vec3 right = window_context->camera.right();
        glm::vec3 up = window_context->camera.up();
        switch (key) {
          case GLFW_KEY_ESCAPE: {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
          }
          case GLFW_KEY_W: {
            window_context->camera.position(position + front * 1.f);
            break;
          }
          case GLFW_KEY_A: {
            window_context->camera.position(position + right * 1.f);
            break;
          }
          case GLFW_KEY_S: {
            window_context->camera.position(position - front * 1.f);
            break;
          }
          case GLFW_KEY_D: {
            window_context->camera.position(position - right * 1.f);
            break;
          }
          case GLFW_KEY_SPACE: {
            window_context->camera.position(position + up * 1.f);
            break;
          }
          case GLFW_KEY_LEFT_SHIFT: {
            window_context->camera.position(position - up * 1.f);
            break;
          }
          case GLFW_KEY_RIGHT_SHIFT: {
            window_context->camera.position(position - up * 1.f);
            break;
          }
        }
      });

      glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        context* window_context = (context*) glfwGetWindowUserPointer(window);

        float xoffset = xpos - window_context->last_mouse_x;
        float yoffset = window_context->last_mouse_y - ypos; // reversed since y-coordinates range from bottom to top
        window_context->last_mouse_x = xpos;
        window_context->last_mouse_y = ypos;

        const float sensitivity = .1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        float yaw = window_context->camera.yaw() + xoffset;
        float pitch = window_context->camera.pitch() + yoffset;

        if (pitch > 89.f) {
          pitch = 89.f;
        }
        if (pitch < -89.f) {
          pitch = -89.f;
        }
        window_context->camera.rotate(yaw, pitch);
      });

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glEnable(GL_DEPTH_TEST);

      skybox space({
        "..\\res\\models\\skybox\\right.png",
        "..\\res\\models\\skybox\\left.png",
        "..\\res\\models\\skybox\\bottom.png",
        "..\\res\\models\\skybox\\top.png",
        "..\\res\\models\\skybox\\front.png",
        "..\\res\\models\\skybox\\back.png"
      });

      mesh sun(
        "..\\res\\models\\shapes\\sphere.obj", 
        material{
          .color = glm::vec3(.9f, .3f, .1f),
          .emissive = 1.f,
          .ambiental = 1.f,
          .diffusive = 0.f,
          .specular = 0.f,
          .shiness = 1.f
        }
      );
      mesh moon(
        "..\\res\\models\\shapes\\sphere.obj",
        material{
          .color = glm::vec3(1.f),
          .emissive = .3f,
          .ambiental = 1.f,
          .diffusive = 1.f,
          .specular = .2f,
          .shiness = 1.f
        }
      );
      mesh house(
        "..\\res\\models\\shapes\\cube.obj",
        material{
          .color = glm::vec3(1.f),
          .emissive = 0.f,
          .ambiental = 1.f,
          .diffusive = 1.f,
          .specular = .2f,
          .shiness = 1.f
        }
      );
      mesh floating(
        "..\\res\\models\\shapes\\cube.obj",
        material{
          .color = glm::vec3(1.f),
          .emissive = 0.f,
          .ambiental = 1.f,
          .diffusive = 1.f,
          .specular = .8f,
          .shiness = 1.f
        }
      );
      mesh plane(
        "..\\res\\models\\shapes\\cylinder.obj",
        material{
          .color = glm::vec3(1.f),
          .emissive = 0.f,
          .ambiental = 1.f,
          .diffusive = 1.f,
          .specular = .5f,
          .shiness = 1.f
        }
      );

      sun.position(glm::vec3(160, 64, 0));
      sun.scale(glm::vec3(12));
      sun.normal_map("..\\res\\models\\sun\\sun_normal_map.png");

      moon.position(glm::vec3(-160, 64, 0));
      moon.scale(glm::vec3(12));
      moon.albedo("..\\res\\models\\moon\\moon_texture.png");
      moon.normal_map("..\\res\\models\\moon\\moon_normal_map.png");

      house.position(glm::vec3(0, 8, 0));
      house.scale(glm::vec3(8));

      floating.position(glm::vec3(20, 16, 0));
      floating.scale(glm::vec3(3));

      plane.scale(glm::vec3(256, 1, 256));

      window_context.camera.position(glm::vec3(0, 128, 0));

      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      static double limitFPS = 1.0 / 60.0;

      double lastTime = glfwGetTime();
      double timer = lastTime;
      double deltaTime = 0;
      double nowTime = 0;

      program shadow_map_program;
      shadow_map_program.add_shader("..\\res\\shaders\\shadow\\shadow.geom", GL_GEOMETRY_SHADER);
      shadow_map_program.add_shader("..\\res\\shaders\\shadow\\shadow.vert", GL_VERTEX_SHADER);
      shadow_map_program.add_shader("..\\res\\shaders\\shadow\\shadow.frag", GL_FRAGMENT_SHADER);
      shadow_map_program.link();

      float degree = 0;      
      while (!glfwWindowShouldClose(window)) {
        nowTime = glfwGetTime();
        deltaTime += (nowTime - lastTime) / limitFPS;
        lastTime = nowTime;

        while (deltaTime >= 1.0) {
          sun.revolution(glm::vec3(0, degree / 8, 0));
          moon.revolution(glm::vec3(0, degree / 8, 0));
          floating.rotation(glm::vec3(0, degree / 2, 0));
          floating.revolution(glm::vec3(0, degree / 2, 0));
          degree += 1;
          --deltaTime;
        }

        window_context.point_lights[0].position = glm::vec3(sun.modelToWorld() * glm::vec4(.0, .0, .0, 1.0));
        window_context.point_lights[1].position = glm::vec3(moon.modelToWorld() * glm::vec4(.0, .0, .0, 1.0));

        shadow_map_program.use();
        for (int i = 0; point_light& light : window_context.point_lights) {
          light.shadow.begin();
          shadow_map_program.uniform_mat4("uLightProjection", light.lightProjection());
          for (int j = 0; glm::mat4& view : light.worldToLight()) {
            shadow_map_program.uniform_mat4("uWorldToLight[" + std::to_string(j++) + "]", view);
          }
          shadow_map_program.uniform_vec3("uLightPosition", light.position);
          shadow_map_program.uniform_float("uFarPlane", light.far_plane);
          floating.draw_shadow(shadow_map_program);
          house.draw_shadow(shadow_map_program);
          plane.draw_shadow(shadow_map_program);
          light.shadow.end();
          light.shadow.bind(GL_TEXTURE2 + i++);
        }

        glViewport(0, 0, window_context.viewport_width, window_context.viewport_height);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        sun.draw(window_context, degree / 48);
        moon.draw(window_context);
        floating.draw(window_context);
        house.draw(window_context);
        plane.draw(window_context);
        space.draw(window_context);

        glfwSwapBuffers(window);
        glfwPollEvents();
      }
    }
  }
  glfwTerminate();
}
