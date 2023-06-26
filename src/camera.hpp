#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

  class camera {
    public:
      camera(float fov, float width, float height, float near_clip_plane, float far_clip_plane, const glm::vec3& world_up);
      glm::mat4 world_to_camera_matrix() const noexcept;
      glm::mat4 projection_matrix() const noexcept;
      void position(const glm::vec3& value) noexcept;
      glm::vec3 position() const noexcept;
      void rotate(const float x, const float y) noexcept;
      float yaw() const noexcept;
      float pitch() const noexcept;
      glm::vec3 front() const noexcept;
      glm::vec3 right() const noexcept;
      glm::vec3 up() const noexcept;
    private:
      float _fov;
      glm::vec3 _position;
      glm::vec3 _front;
      glm::vec3 _right;
      glm::vec3 _up;
      float _pitch; // X axis rotation
      float _yaw; // Y axis rotation
      float _width;
      float _height;
      glm::vec3 _world_up;
      float _near_clip_plane;
      float _far_clip_plane;
  };
