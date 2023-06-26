#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>

class program {
  public:
    program();

    void add_shader(const std::string& source, GLenum type);

    void link();

    void use();

    void uniform_int(const std::string& uniform, int value);

    void uniform_float(const std::string& uniform, float value);

    void uniform_mat4(const std::string& uniform, const glm::mat4& value);

    void uniform_vec3(const std::string& uniform, const glm::vec3& value);

    int uniform_location(const std::string& uniform);

    static const std::string load_source_from_file(const std::string& source);

  private:
    std::unordered_map<std::string, int> _uniform_location;
    GLuint _handle;
};
