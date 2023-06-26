#pragma once

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <array>
#include "program.hpp"

struct context;

class skybox {
public:
  skybox(const std::array<std::string, 6> sides);
  void draw(context& c);

private:
  GLuint _texture;
  int _projection_uniform_location;
  int _view_uniform_location;
  int _skybox_uniform_location;
  GLuint _vao;
  GLuint _vbo;
  program _program;
};
