#include "camera.hpp"

  camera::camera(float fov, float width, float height, float near_clip_plane, float far_clip_plane, const glm::vec3& world_up) {
    _fov = fov;
    _width = width;
    _height = height;
    _near_clip_plane = near_clip_plane;
    _far_clip_plane = far_clip_plane; 
    _world_up = world_up;
    _position = glm::vec3(0.f);
    _front = glm::vec3(0.f, 0.f, -1.f);
    _right = glm::normalize(glm::cross(_world_up, _front));
    _up = glm::normalize(glm::cross(_front, _right));
    _yaw = 0;
    _pitch = 0;
  }

  glm::vec3 camera::position() const noexcept {
    return _position;
  }

  void camera::position(const glm::vec3& value) noexcept {
    _position = value;
  }

  glm::mat4 camera::world_to_camera_matrix() const noexcept {
    return glm::lookAt(
      _position,
      _position + _front,
      _up
    );
  }

  glm::mat4 camera::projection_matrix() const noexcept {
    return glm::perspective(
      _fov,
      _width / _height,
      _near_clip_plane,
      _far_clip_plane
    );
  }

  
  void camera::rotate(const float x, const float y) noexcept {
    _yaw = x;
    _pitch = y;
    _front = glm::normalize(glm::vec3(
      cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
      sin(glm::radians(_pitch)),
      sin(glm::radians(_yaw)) * cos(glm::radians(_pitch))
    ));
    _right = glm::normalize(glm::cross(_world_up, _front));
    _up = glm::normalize(glm::cross(_front, _right));
  }
  
  float camera::yaw() const noexcept {
    return _yaw;
  }

  float camera::pitch() const noexcept {
    return _pitch;
  }

  glm::vec3 camera::up() const noexcept {
    return _up;
  }

  glm::vec3 camera::front() const noexcept {
    return _front;
  }

  glm::vec3 camera::right() const noexcept {
    return _right;
  }