#include "context.hpp"
#include "skybox.hpp"

#include <stb/stb_image.h>

skybox::skybox(const std::array<std::string, 6> sides) {
  const GLfloat vertices[] = {
    -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,
    1.f, -1.f, -1.f,
    1.f, -1.f, -1.f,
    1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f, -1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
    -1.f, -1.f,  1.f,

    1.f, -1.f, -1.f,
    1.f, -1.f,  1.f,
    1.f,  1.f,  1.f,
    1.f,  1.f,  1.f,
    1.f,  1.f, -1.f,
    1.f, -1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f,  1.f,  1.f,
    1.f,  1.f,  1.f,
    1.f,  1.f,  1.f,
    1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,

    -1.f,  1.f, -1.f,
    1.f,  1.f, -1.f,
    1.f,  1.f,  1.f,
    1.f,  1.f,  1.f,
    -1.f,  1.f,  1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
    1.f, -1.f, -1.f,
    1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
    1.f, -1.f,  1.f
  };

  _program.add_shader("..\\res\\shaders\\skybox\\skybox.vert", GL_VERTEX_SHADER);
  _program.add_shader("..\\res\\shaders\\skybox\\skybox.frag", GL_FRAGMENT_SHADER);

  _program.link();

  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

  for (int i = 0; std::string side : sides) {
    int width = 0;
    int height = 0;
    stbi_set_flip_vertically_on_load(true);
    int channels = 0;
    uint8_t* data = stbi_load(side.c_str(), &width, &height, &channels, 4);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    delete data;
  }
}

void skybox::draw(context& c) {
  glDepthFunc(GL_LEQUAL);
  _program.use();
  _program.uniform_mat4("uProjection", c.camera.projection_matrix());
  _program.uniform_mat4("uView", glm::mat4(glm::mat3(c.camera.world_to_camera_matrix())));
  _program.uniform_int("uSkybox", 0);
  glBindVertexArray(_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthFunc(GL_LESS);
}