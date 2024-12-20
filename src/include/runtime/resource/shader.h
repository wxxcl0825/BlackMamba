#pragma once

#include "common/common.h"

#include <string>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void use() const;

  GLuint getProgram() const { return _program; }

  void setUniform(const std::string &name, float value) const;
  void setUniform(const std::string &name, int value) const;
  void setUniform(const std::string &name, glm::vec3 value) const;
  void setUniform(const std::string &name, glm::mat3 value) const;
  void setUniform(const std::string &name, glm::mat4 value) const;

private:
  enum class ErrorType { COMPILE, LINK };

  GLuint _program{0};

  static void checkShaderErrors(GLuint target, ErrorType type);
};