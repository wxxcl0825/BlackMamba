#pragma once

#include "common/common.h"
#include "common/macro.h"
#include "geometry/geometry.h"
#include "runtime/resource/texture/texture.h"
#include "runtime/resource/shader/shader.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/model.h"

#include <unordered_map>

class ResourceManager{
public:
    ~ResourceManager();
    static ResourceManager *getResourceManager();

    Texture *loadTexture(const std::string& filePath) const;
    Texture *loadTexture(const std::vector<std::string>& filePaths) const;

    Shader *loadShader(const std::string& vertexPath, const std::string& fragmentPath) const;

    Geometry *createBoxGeometry(float size) const;
    Geometry *createPlaneGeometry(float width, float height) const;
    Geometry *createSphereGeometry(float radius) const;
    Geometry *loadGeometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<unsigned int>& indices) const;

    Model *loadModel(const std::string& filePath) const;

private:
    static ResourceManager *_instance;
    static std::unordered_map<std::string, Texture *> _textureMap;
    static std::unordered_map<std::string, Shader *> _shaderMap;
    static std::unordered_map<std::string, Model *> _modelMap;

    static std::vector<Geometry *> _geometryList;
    ResourceManager();

};