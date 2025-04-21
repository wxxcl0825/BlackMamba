#include "runtime/resource/geometry/geometry.h"

#include "common/macro.h"

Geometry::Geometry() {}

Geometry::Geometry(const std::vector<glm::vec3> &vertices,
                   const std::vector<glm::vec2> &uvs,
                   const std::vector<glm::vec3> &normals,
                   const std::vector<unsigned int> &indices) {
  _numIndices = indices.size();

  Log("Creating geometry with %ld vertices, %ld uvs, %ld normals, %d indices",
      vertices.size(), uvs.size(), normals.size(), _numIndices);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  uploadBuffer(_posVbo, vertices, 0, 3);
  uploadBuffer(_uvVbo, uvs, 1, 2);
  uploadBuffer(_normalVbo, normals, 2, 3);

  Log("Binding ebo: %d", _ebo);
  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Geometry::Geometry(const std::vector<glm::vec3> &vertices,
                   const std::vector<glm::vec2> &uvs) {
  _numIndices = 0;

  Log("Creating geometry with %ld vertices, %ld uvs", vertices.size(),
      uvs.size());

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  uploadBuffer(_posVbo, vertices, 0, 3);
  uploadBuffer(_uvVbo, uvs, 1, 2);

  glBindVertexArray(0);
}

Geometry::~Geometry() {
  Log("Deleting geometry with vao: %d, posVbo: %d, uvVbo: %d, normalVbo: %d, "
      "ebo: %d",
      _vao, _posVbo, _uvVbo, _normalVbo, _ebo);

  if (_vao != 0)
    glDeleteVertexArrays(1, &_vao);
  if (_posVbo != 0)
    glDeleteBuffers(1, &_posVbo);
  if (_uvVbo != 0)
    glDeleteBuffers(1, &_uvVbo);
  if (_normalVbo != 0)
    glDeleteBuffers(1, &_normalVbo);
  if (_ebo != 0)
    glDeleteBuffers(1, &_ebo);
}

template <typename T>
void Geometry::uploadBuffer(GLuint &buffer, const std::vector<T> &data,
                            GLuint attributeIndex, GLint componentCount) {
  GL_CALL(glGenBuffers(1, &buffer));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                       GL_STATIC_DRAW));
  GL_CALL(glEnableVertexAttribArray(attributeIndex));
  GL_CALL(glVertexAttribPointer(attributeIndex, componentCount, GL_FLOAT,
                                GL_FALSE, componentCount * sizeof(float),
                                (void *)0));
}