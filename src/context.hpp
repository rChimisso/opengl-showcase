#pragma once

#include "camera.hpp"
#include "light.hpp"
#include <array>

struct context {
  static constexpr int total_lights = 2;
  camera camera;
  double last_mouse_x;
  double last_mouse_y;
  ambient_light ambient_light;
  std::array<point_light, total_lights> point_lights;
  unsigned int viewport_width;
  unsigned int viewport_height;
};
