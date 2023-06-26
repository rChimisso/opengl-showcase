#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>

class point_light_shadow {
  public:
    point_light_shadow(GLuint width = 4096, GLuint height = 4096);
    
    void begin();
    void end();

    float aspect();

    void bind(GLenum index);

  private:
    GLuint _width;
    GLuint _height;
    GLuint _framebuffer;
    GLuint _texture;
};
