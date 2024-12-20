#pragma once

#include "common/common.h" 
#include <cstdint>
#include <vector>

class Geometry {
    friend class ResourceManager;

public:
    GLuint getVao() const { return _vao; }
    GLuint getPosVbo() const { return _posVbo; }
    GLuint getUvVbo() const { return _uvVbo; }
    GLuint getNormalVbo() const { return _normalVbo; }
    GLuint getEbo() const { return _ebo; }
    uint32_t getNumIndices() const { return _numIndices; }

private:
    GLuint _vao{0};
    GLuint _posVbo{0};
    GLuint _uvVbo{0};
    GLuint _normalVbo{0};
    GLuint _ebo{0};

    uint32_t _numIndices{0};

    Geometry();
    Geometry(
        const std::vector<glm::vec3>& vertices, 
        const std::vector<glm::vec2>& uvs, 
        const std::vector<glm::vec3>& normals, 
        const std::vector<unsigned int>& indices);
    ~Geometry();
};