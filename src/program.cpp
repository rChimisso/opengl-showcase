#include "program.hpp"
#include <vector>

program::program() {
  _handle = glCreateProgram();
}

void program::add_shader(const std::string& source, GLenum type) {
  GLuint shader = glCreateShader(type);
  std::string code = program::load_source_from_file(source);
  const char* shader_source = code.c_str();
  glShaderSource(shader, 1, &shader_source, nullptr);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    std::cout << source << std::endl;
    for (auto l : errorLog) {
      std::cout << l;
    }
  }

  glAttachShader(_handle, shader);
  glDeleteShader(shader);
}

void program::link() {
  glLinkProgram(_handle);
}

void program::use() {
  glUseProgram(_handle);
}

void program::uniform_int(const std::string& uniform, int value) {
  glUniform1i(uniform_location(uniform), value);
}

void program::uniform_float(const std::string& uniform, float value) {
  glUniform1f(uniform_location(uniform), value);
}

void program::uniform_mat4(const std::string& uniform, const glm::mat4& value) {
  glUniformMatrix4fv(uniform_location(uniform), 1, GL_FALSE, &value[0][0]);
}

void program::uniform_vec3(const std::string& uniform, const glm::vec3& value) {
  glUniform3f(uniform_location(uniform), value.x, value.y, value.z);
}

int program::uniform_location(const std::string& uniform) {
  if (_uniform_location.find(uniform) == _uniform_location.end()) {
    int location = glGetUniformLocation(_handle, uniform.c_str());
    _uniform_location[uniform] = location;
    return location;
  }
  return _uniform_location[uniform];
}

const std::string program::load_source_from_file(const std::string& source) {
  std::ifstream file(source);
  std::stringstream stream;
  std::string line;
  while (std::getline(file, line)) {
    stream << line << "\n";
  }
  const std::string data = stream.str();
  return data;
}
