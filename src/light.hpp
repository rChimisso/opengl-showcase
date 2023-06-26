#pragma once

#include <glm/glm.hpp>
#include <array>
#include "shadow.hpp"

static constexpr glm::vec3 color_white = glm::vec3(1.f, 1.f, 1.f);

struct ambient_light {
  glm::vec3 color;
  float intensity;
};

struct point_light {
  glm::vec3 color;
  glm::vec3 position;

  float diffusive;
  float specular;

  float constant;
  float linear;
  float quadratic;

  float far_plane = 512.f;

  point_light_shadow shadow;

  glm::mat4 lightProjection() {
    return glm::perspective(glm::radians(90.f), shadow.aspect(), .1f, far_plane);
  }

  std::array<glm::mat4, 6> worldToLight() {
    return {
      glm::lookAt(position, position + glm::vec3(1.0, .0, .0), glm::vec3(.0, -1.0, .0)),
      glm::lookAt(position, position + glm::vec3(-1.0, .0, .0), glm::vec3(.0, -1.0, .0)),
      glm::lookAt(position, position + glm::vec3(.0, 1.0, .0), glm::vec3(.0, .0, 1.0)),
      glm::lookAt(position, position + glm::vec3(.0, -1.0, .0), glm::vec3(.0, .0, -1.0)),
      glm::lookAt(position, position + glm::vec3(.0, .0, 1.0), glm::vec3(.0, -1.0, .0)),
      glm::lookAt(position, position + glm::vec3(.0, .0, -1.0), glm::vec3(.0, -1.0, .0))
    };
  }
};
