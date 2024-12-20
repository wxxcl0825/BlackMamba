#include "runtime/resource/geometry/geometry.h"

#include "common/macro.h"

Geometry::Geometry(){

}

Geometry::Geometry(
        const std::vector<glm::vec3>& vertices, 
        const std::vector<glm::vec2>& uvs, 
        const std::vector<glm::vec3>& normals, 
        const std::vector<unsigned int>& indices
){
    _numIndices = indices.size();

    Log("Creating geometry with %d vertices, %d uvs, %d normals, %d indices", vertices.size(), uvs.size(), normals.size(), _numIndices);

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_posVbo);
    glGenBuffers(1, &_uvVbo);
    glGenBuffers(1, &_normalVbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    Log("Binding posVbo: %d", _posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    Log("Binding uvVbo: %d", _uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _uvVbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    Log("Binding normalVbo: %d", _normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _normalVbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    Log("Binding ebo: %d", _ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}