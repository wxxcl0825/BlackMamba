#include "runtime/resource/resourceManager.h"

#include "common/macro.h"
#include "runtime/resource/audio/audio.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/texture/texture.h"

ResourceManager::~ResourceManager() {
  for (auto it = _textureMap.begin(); it != _textureMap.end(); it++) {
    delete it->second;
  }
  _textureMap.clear();

  for (auto it = _shaderMap.begin(); it != _shaderMap.end(); it++) {
    delete it->second;
  }
  _shaderMap.clear();

  for (auto it = _geometryList.begin(); it != _geometryList.end(); it++) {
    delete *it;
  }
  _geometryList.clear();
}

Texture *ResourceManager::loadTexture(const std::string &filePath) {
  if (_textureMap.find(filePath) == _textureMap.end()) {
    Log("Loading texture: %s", filePath.c_str());
    Texture *_texture = new Texture(filePath, 0);
    _textureMap[filePath] = _texture;
  }
  return _textureMap[filePath];
}

Texture *
ResourceManager::loadTexture(const std::vector<std::string> &filePaths) {
  if (_textureMap.find(filePaths[0]) == _textureMap.end()) {
    Log("Loading texture: %s", filePaths[0].c_str());
    Texture *_texture = new Texture(filePaths, 0);
    _textureMap[filePaths[0]] = _texture;
  }
  return _textureMap[filePaths[0]];
}

Texture *ResourceManager::loadTexture(const std::string &filePath,
                                      unsigned char *dataIn, uint32_t widthIn,
                                      uint32_t heightIn) {
  if (_textureMap.find(filePath) == _textureMap.end()) {
    Log("Loading texture: %s", filePath.c_str());
    Texture *_texture = new Texture(dataIn, widthIn, heightIn, 0);
    _textureMap[filePath] = _texture;
  }
  return _textureMap[filePath];
}

Shader *ResourceManager::loadShader(const std::string &vertexPath,
                                    const std::string &fragmentPath) {
  if (_shaderMap.find(vertexPath + fragmentPath) == _shaderMap.end()) {
    Log("Loading shader: %s and %s", vertexPath.c_str(), fragmentPath.c_str());
    Shader *_shader = new Shader(vertexPath, fragmentPath);
    _shaderMap[vertexPath + fragmentPath] = _shader;
  }
  return _shaderMap[vertexPath + fragmentPath];
}

Shader *ResourceManager::loadShader(const std::string &vertexPath,
                                    const std::string &geometryPath,
                                    const std::string &fragmentPath) {
  if (_shaderMap.find(vertexPath + geometryPath + fragmentPath) ==
      _shaderMap.end()) {
    Log("Loading shader: %s, %s and %s", vertexPath.c_str(),
        geometryPath.c_str(), fragmentPath.c_str());
    Shader *_shader = new Shader(vertexPath, geometryPath, fragmentPath);
    _shaderMap[vertexPath + geometryPath + fragmentPath] = _shader;
  }
  return _shaderMap[vertexPath + geometryPath + fragmentPath];
}

Shader *ResourceManager::loadShader(const std::string &vertexPath,
                                    const std::string &tessCtrlPath,
                                    const std::string &tessEvalPath,
                                    const std::string &fragmentPath) {
  if (_shaderMap.find(vertexPath + tessCtrlPath + tessEvalPath +
                      fragmentPath) == _shaderMap.end()) {
    Log("Loading shader: %s, %s, %s and %s", vertexPath.c_str(),
        tessCtrlPath.c_str(), tessEvalPath.c_str(), fragmentPath.c_str());
    Shader *_shader =
        new Shader(vertexPath, tessCtrlPath, tessEvalPath, fragmentPath);
    _shaderMap[vertexPath + tessCtrlPath + tessEvalPath + fragmentPath] =
        _shader;
  }
  return _shaderMap[vertexPath + tessCtrlPath + tessEvalPath + fragmentPath];
}

// front back top bottom left right
Geometry *ResourceManager::createBoxGeometry(float size) {
  std::vector<glm::vec3> vertices = {glm::vec3(size / 2, size / 2, size / 2),
                                     glm::vec3(-size / 2, size / 2, size / 2),
                                     glm::vec3(-size / 2, size / 2, -size / 2),
                                     glm::vec3(size / 2, size / 2, -size / 2),

                                     glm::vec3(size / 2, -size / 2, size / 2),
                                     glm::vec3(-size / 2, -size / 2, size / 2),
                                     glm::vec3(-size / 2, -size / 2, -size / 2),
                                     glm::vec3(size / 2, -size / 2, -size / 2),

                                     glm::vec3(size / 2, size / 2, size / 2),
                                     glm::vec3(-size / 2, size / 2, size / 2),
                                     glm::vec3(-size / 2, -size / 2, size / 2),
                                     glm::vec3(size / 2, -size / 2, size / 2),

                                     glm::vec3(size / 2, size / 2, -size / 2),
                                     glm::vec3(-size / 2, size / 2, -size / 2),
                                     glm::vec3(-size / 2, -size / 2, -size / 2),
                                     glm::vec3(size / 2, -size / 2, -size / 2),

                                     glm::vec3(size / 2, size / 2, size / 2),
                                     glm::vec3(size / 2, size / 2, -size / 2),
                                     glm::vec3(size / 2, -size / 2, -size / 2),
                                     glm::vec3(size / 2, -size / 2, size / 2),

                                     glm::vec3(-size / 2, size / 2, size / 2),
                                     glm::vec3(-size / 2, size / 2, -size / 2),
                                     glm::vec3(-size / 2, -size / 2, -size / 2),
                                     glm::vec3(-size / 2, -size / 2, size / 2)};

  // six faces, each face has 4 vertices
  std::vector<glm::vec2> uvs = {
      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
  };

  std::vector<glm::vec3> normals = {
      glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),

      glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),

      glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),

      glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
      glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),

      glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),

      glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)};

  std::vector<unsigned int> indices = {
      0,  1,  2,  0,  2,  3,  4,  6,  5,  4,  7,  6,  8,  9,  10, 8,  10, 11,
      12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 21, 22, 20, 22, 23};

  Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);

  _geometryList.push_back(_geometry);

  return _geometry;
}

Geometry *ResourceManager::createPlaneGeometry(float width, float height) {
  std::vector<glm::vec3> vertices = {glm::vec3(width / 2, 0.0f, height / 2),
                                     glm::vec3(-width / 2, 0.0f, height / 2),
                                     glm::vec3(-width / 2, 0.0f, -height / 2),
                                     glm::vec3(width / 2, 0.0f, -height / 2)};

  std::vector<glm::vec2> uvs = {glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
                                glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};

  std::vector<glm::vec3> normals = {
      glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

  Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);

  _geometryList.push_back(_geometry);

  return _geometry;
}

Geometry *
ResourceManager::loadGeometry(const std::vector<glm::vec3> &vertices,
                              const std::vector<glm::vec2> &uvs,
                              const std::vector<glm::vec3> &normals,
                              const std::vector<unsigned int> &indices) {
  Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);

  _geometryList.push_back(_geometry);

  return _geometry;
}

Geometry *ResourceManager::loadGeometry(const std::vector<glm::vec3> &vertices,
                                        const std::vector<glm::vec2> &uvs) {
  Geometry *_geometry = new Geometry(vertices, uvs);

  _geometryList.push_back(_geometry);

  return _geometry;
}

Audio *ResourceManager::loadAudio(const std::string &filePath) {
  if (_audioMap.find(filePath) == _audioMap.end()) {
    Log("Loading audio: %s", filePath.c_str());
    Audio *_audio = new Audio(filePath);
    _audioMap[filePath] = _audio;
  }
  return _audioMap[filePath];
}