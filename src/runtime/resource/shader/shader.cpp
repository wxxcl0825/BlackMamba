#include "runtime/resource/shader/shader.h"

#include "common/macro.h"
#include <fstream>
#include <sstream>

void Shader::checkShaderErrors(GLuint target, ErrorType type) {
  int success = 0;
  int infoLogLength = 0;

  if (type == ErrorType::COMPILE) {
    glGetShaderiv(target, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::vector<char> infoLog(infoLogLength);
      glGetShaderInfoLog(target, infoLogLength, NULL, infoLog.data());
      Err("Shader compile error: %s", infoLog.data());
    }
  } else if (type == ErrorType::LINK) {
    glGetProgramiv(target, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::vector<char> infoLog(infoLogLength);
      glGetProgramInfoLog(target, infoLogLength, NULL, infoLog.data());
      Err("Shader link error: %s", infoLog.data());
    }
  }
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  linkProgram({vertex, fragment});
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::Shader(const std::string &vertexPath, const std::string &geometryPath,
               const std::string &fragmentPath) {
  GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  GLuint geometry = compileShader(geometryPath, GL_GEOMETRY_SHADER);
  GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  linkProgram({vertex, geometry, fragment});
  glDeleteShader(vertex);
  glDeleteShader(geometry);
  glDeleteShader(fragment);
}

Shader::~Shader() {
  if (_program)
    GL_CALL(glDeleteProgram(_program));
}

void Shader::begin() const { GL_CALL(glUseProgram(_program)); }

void Shader::end() const { GL_CALL(glUseProgram(0)); }

void Shader::setUniform(const std::string &name, int value) const {
  GLint location = GL_CALL(glGetUniformLocation(_program, name.c_str()));
  GL_CALL(glUniform1i(location, value));
}

void Shader::setUniform(const std::string &name, float value) const {
  GLint location = GL_CALL(glGetUniformLocation(_program, name.c_str()));
  GL_CALL(glUniform1f(location, value));
}

void Shader::setUniform(const std::string &name, glm::vec3 value) const {
  GLint location = GL_CALL(glGetUniformLocation(_program, name.c_str()));
  GL_CALL(glUniform3fv(location, 1, glm::value_ptr(value)));
}

void Shader::setUniform(const std::string &name, glm::mat3 value) const {
  GLint location = GL_CALL(glGetUniformLocation(_program, name.c_str()));
  GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::setUniform(const std::string &name, glm::mat4 value) const {
  GLint location = GL_CALL(glGetUniformLocation(_program, name.c_str()));
  GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
}

GLuint Shader::compileShader(const std::string &path, GLenum shaderType) {
  std::ifstream file;
  std::string code;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path.c_str());
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    Err("Shader file error: %s", e.what());
  }
  GLuint shader = glCreateShader(shaderType);
  const char *source = code.c_str();
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  checkShaderErrors(shader, ErrorType::COMPILE);
  return shader;
}

void Shader::linkProgram(const std::vector<GLuint> &shaders) {
  _program = glCreateProgram();
  for (auto s : shaders) {
    glAttachShader(_program, s);
  }
  glLinkProgram(_program);
  checkShaderErrors(_program, ErrorType::LINK);
}