#pragma once

#include "common/common.h"

#include <string>
#include <vector>

class Shader {
  friend class ResourceManager;

public:
  void begin() const;
  void end() const;

  GLuint getProgram() const { return _program; }

  void setUniform(const std::string &name, float value) const;
  void setUniform(const std::string &name, int value) const;
  void setUniform(const std::string &name, glm::vec3 value) const;
  void setUniform(const std::string &name, glm::mat3 value) const;
  void setUniform(const std::string &name, glm::mat4 value) const;

private:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  Shader(const std::string &vertexPath, const std::string &geometryPath, const std::string &fragmentPath);
  ~Shader();

  enum class ErrorType { COMPILE, LINK };

  GLuint _program{0};

  static void checkShaderErrors(GLuint target, ErrorType type);

  GLuint compileShader(const std::string &path, GLenum shaderType);
  void linkProgram(const std::vector<GLuint> &shaders);
};