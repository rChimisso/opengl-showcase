#pragma once
#include <GL/glew.h>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp> // Assimp Importer object
#include <vector>
#include <assimp/scene.h>       // Assimp output data structure
#include <assimp/postprocess.h> // Assimp post processing flags
#include "context.hpp"
#include "program.hpp"
#include <memory>
#include <fstream>
#include <sstream>

#define DEBUG(func) while(glGetError() != GL_NO_ERROR); \
  func; \
  while (GLuint error = glGetError()) { \
    std::cout << "Error \"" << error << "\" at " << __FILE__ << " in " << #func << " line " << __LINE__ << std::endl; \
  }

#include <glm/glm.hpp>
#include <iostream>

struct vertex {
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
  glm::vec3 normal_tangent;
  glm::vec3 normal_bitangent;
};

struct material {
  glm::vec3 color;
  float emissive;
  float ambiental;
  float diffusive;
  float specular;
  float shiness;
};

class mesh {
  public:
    void draw_shadow(program program) noexcept {
      program.uniform_mat4("uModelToWorld", modelToWorld());
      DEBUG(glBindVertexArray(_vertex_array));
      DEBUG(glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr));
    }

    glm::mat4 modelToWorld() {
      glm::mat4 model_to_world = glm::mat4(1.f);
      model_to_world = glm::translate(model_to_world, _revolution_center);
      model_to_world = glm::rotate(model_to_world, glm::radians(_revolution.x), glm::vec3(1.f, 0.f, 0.f));
      model_to_world = glm::rotate(model_to_world, glm::radians(_revolution.y), glm::vec3(0.f, 1.f, 0.f));
      model_to_world = glm::rotate(model_to_world, glm::radians(_revolution.z), glm::vec3(0.f, 0.f, 1.f));
      model_to_world = glm::translate(model_to_world, _position);
      model_to_world = glm::rotate(model_to_world, glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
      model_to_world = glm::rotate(model_to_world, glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
      model_to_world = glm::rotate(model_to_world, glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));
      model_to_world = glm::scale(model_to_world, _scale);
      return model_to_world;
    }

    void draw(context& context, float time = -1.f) noexcept {
      DEBUG(glBindVertexArray(_vertex_array));
      _program.use();

      DEBUG(glActiveTexture(GL_TEXTURE0));
      DEBUG(glBindTexture(GL_TEXTURE_2D, _normal_map));

      DEBUG(glActiveTexture(GL_TEXTURE1));
      DEBUG(glBindTexture(GL_TEXTURE_2D, _albedo));
      
      _program.uniform_int("uNormalMap", 0);
      _program.uniform_int("uAlbedo", 1);
      _program.uniform_float("uTime", time);

      _program.uniform_mat4("uModelToWorld", modelToWorld());
      _program.uniform_mat4("uWorldToCamera", context.camera.world_to_camera_matrix());
      _program.uniform_mat4("uCameraProjection", context.camera.projection_matrix());

      glm::vec3 camera_position = context.camera.position();

      for (int i = 0; point_light& light : context.point_lights) {
        std::string index = std::to_string(i);
        _program.uniform_vec3("uPointLights[" + index + "].color", light.color);
        _program.uniform_vec3("uPointLights[" + index + "].position", light.position);
        _program.uniform_float("uPointLights[" + index + "].far_plane", light.far_plane);
        _program.uniform_float("uPointLights[" + index + "].diffusive", light.diffusive);
        _program.uniform_float("uPointLights[" + index + "].specular", light.specular);
        _program.uniform_float("uPointLights[" + index + "].constant", light.constant);
        _program.uniform_float("uPointLights[" + index + "].linear", light.linear);
        _program.uniform_float("uPointLights[" + index + "].quadratic", light.quadratic);
        _program.uniform_int("uPointLights[" + index + "].shadowMap", 2 + i++);
      }

      _program.uniform_vec3("uCameraPosition", camera_position);

      _program.uniform_vec3("uMaterial.color", _material.color);
      _program.uniform_float("uMaterial.emissive", _material.emissive);
      _program.uniform_float("uMaterial.ambiental", _material.ambiental);
      _program.uniform_float("uMaterial.diffusive", _material.diffusive);
      _program.uniform_float("uMaterial.specular", _material.specular);
      _program.uniform_float("uMaterial.shiness", _material.shiness);

      _program.uniform_vec3("uAmbientLight.color", context.ambient_light.color);
      _program.uniform_int("uAmbientLight.intensity", context.ambient_light.intensity);

      DEBUG(glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr));
    }

    mesh(const std::string& obj, material material) noexcept {
      _material = material;
      Assimp::Importer Importer;
      const aiScene* scene = Importer.ReadFile(obj.c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate);
      if (scene) {
        const aiMesh* mesh = scene->mMeshes[0];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
          aiVector3D position = mesh->mVertices[i];
          aiVector3D normal = mesh->mNormals[i];
          aiVector3D texture_coord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.f, 0.f, 0.f);
          aiVector3D tangent = aiVector3D(0, 0, 0);
          aiVector3D bitangent = aiVector3D(0, 0, 0);
          if (mesh->HasTangentsAndBitangents()) {
            tangent = mesh->mTangents[i];
            bitangent = mesh->mBitangents[i];
          }

          _vertices.push_back(
            vertex(
              glm::vec3(position.x, position.y, position.z),
              glm::vec2(texture_coord.x, texture_coord.y),
              glm::vec3(normal.x, normal.y, normal.z),
              glm::vec3(tangent.x, tangent.y, tangent.z),
              glm::vec3(bitangent.x, bitangent.y, bitangent.z)
            )
          );
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
          const aiFace& face = mesh->mFaces[i];
          assert(face.mNumIndices == 3);
          _indices.push_back(face.mIndices[0]);
          _indices.push_back(face.mIndices[1]);
          _indices.push_back(face.mIndices[2]);
        }

        _program.add_shader("..\\res\\shaders\\common\\common.vert", GL_VERTEX_SHADER);
        _program.add_shader("..\\res\\shaders\\common\\common.frag", GL_FRAGMENT_SHADER);
        _program.link();

        DEBUG(glGenVertexArrays(1, &_vertex_array));
        DEBUG(glBindVertexArray(_vertex_array));

        DEBUG(glGenBuffers(1, &_vertex_buffer));
        DEBUG(glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer));
        DEBUG(glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(vertex), _vertices.data(), GL_STATIC_DRAW));

        DEBUG(glGenBuffers(1, &_index_buffer));
        DEBUG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer));
        DEBUG(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW));

        DEBUG(glEnableVertexAttribArray(0));
        DEBUG(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, position)));

        DEBUG(glEnableVertexAttribArray(1));
        DEBUG(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, uv)));

        DEBUG(glEnableVertexAttribArray(2));
        DEBUG(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal)));

        DEBUG(glEnableVertexAttribArray(3));
        DEBUG(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal_tangent)));

        DEBUG(glEnableVertexAttribArray(4));
        DEBUG(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal_bitangent)));

        albedo("..\\res\\models\\default\\white.png");
        normal_map("..\\res\\models\\default\\default_map.png");
      }
    }

    void albedo(const std::string& file) noexcept {
      load_texture(_albedo, file);
    }

    void normal_map(const std::string& file) noexcept {
      load_texture(_normal_map, file);
    }

    glm::vec3 position() noexcept {
      return _position;
    }

    void position(glm::vec3 value) noexcept {
      _position = value;
    }

    void scale(glm::vec3 value) noexcept {
      _scale = value;
    }

    void rotation(glm::vec3 value) noexcept {
      _rotation = value;
    }

    void revolution(glm::vec3 value) noexcept {
      _revolution = value;
    }

    void revolution_center(glm::vec3 value) noexcept {
      _revolution_center = value;
    }

    ~mesh() {
      DEBUG(glDeleteVertexArrays(1, &_vertex_array));
      DEBUG(glDeleteBuffers(1, &_index_buffer));
      DEBUG(glDeleteBuffers(1, &_vertex_buffer));
      DEBUG(glDeleteTextures(1, &_albedo));
      DEBUG(glDeleteTextures(1, &_normal_map));
    }
  private:

    void load_texture(GLuint& handle, const std::string& file) {
      DEBUG(glGenTextures(1, &handle));
      int width = 0;
      int height = 0;
      stbi_set_flip_vertically_on_load(true);
      int channels = 0;
      uint8_t* data = stbi_load(file.c_str(), &width, &height, &channels, 4);
      if (data) {
        DEBUG(glBindTexture(GL_TEXTURE_2D, handle));
        DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        DEBUG(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        DEBUG(glGenerateMipmap(GL_TEXTURE_2D));
      } else {
        DEBUG(glDeleteTextures(1, &handle));
      }
      delete data;
    }

    std::string shader_source(const std::string source) {
      std::ifstream file(source);
      std::stringstream stream;
      std::string line;
      while (std::getline(file, line)) {
        stream << line << "\n";
      }
      const std::string data = stream.str();
      return data;
    }

    program _program;

    GLuint _vertex_array;
    GLuint _index_buffer;
    GLuint _vertex_buffer;

    std::vector<vertex> _vertices;
    std::vector<GLuint> _indices;

    GLuint _albedo;
    GLuint _normal_map;

    glm::vec3 _position = glm::vec3(0.f);
    glm::vec3 _scale = glm::vec3(1.f);
    glm::vec3 _rotation = glm::vec3(0.f);
    glm::vec3 _revolution = glm::vec3(0.f);
    glm::vec3 _revolution_center = glm::vec3(0.f);

    material _material;
};
