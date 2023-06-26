#include "shadow.hpp"

point_light_shadow::point_light_shadow(GLuint width, GLuint height) {
  _width = width;
  _height = height;

  glGenTextures(1, &_texture);

  glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glGenFramebuffers(1, &_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void point_light_shadow::begin() {
  glViewport(0, 0, _width, _height);
  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
}

void point_light_shadow::end() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glCullFace(GL_BACK);
}

void point_light_shadow::bind(GLenum index) {
  glActiveTexture(index);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
}

float point_light_shadow::aspect() {
  return _width / _height;
}
