#pragma once

#include "geometry/geometry.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/shader/shader.h"
#include "runtime/resource/texture/texture.h"

#include <string>
#include <unordered_map>

class ResourceManager {
  friend class Engine;

public:
  ~ResourceManager();

  Texture *loadTexture(const std::string &filePath);
  Texture *loadTexture(const std::vector<std::string> &filePaths);
  Texture *loadTexture(const std::string &filePath, unsigned char *dataIn,
                       uint32_t widthIn, uint32_t heightIn);

  Shader *loadShader(const std::string &vertexPath,
                     const std::string &fragmentPath);
  Shader *loadShader(const std::string &vertexPath,
                     const std::string &geometryPath,
                     const std::string &fragmentPath);

  Geometry *createBoxGeometry(float size);
  Geometry *createPlaneGeometry(float width, float height);

  Geometry *loadGeometry(const std::vector<glm::vec3> &vertices,
                         const std::vector<glm::vec2> &uvs,
                         const std::vector<glm::vec3> &normals,
                         const std::vector<unsigned int> &indices);

private:
  std::unordered_map<std::string, Texture *> _textureMap{};
  std::unordered_map<std::string, Shader *> _shaderMap{};

  std::vector<Geometry *> _geometryList{};

  ResourceManager() = default;
};