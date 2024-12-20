#include "runtime/resource/shader/shader.h"

#include "common/macro.h"
#include <fstream>
#include <sstream>
#include <vector>

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
  std::string vertexCode, fragmentCode;
  std::ifstream vShaderFile, fShaderFile;
  std::stringstream vShaderStream, fShaderStream;

  const char *vertexShaderSource, *fragmentShaderSource;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vShaderFile.open(vertexPath.c_str());
    fShaderFile.open(fragmentPath.c_str());

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    Err("Shader file error: %s", e.what());
  }

  vertexShaderSource = vertexCode.c_str();
  fragmentShaderSource = fragmentCode.c_str();

  GLuint vertex, fragment;
  vertex = glCreateShader(GL_VERTEX_SHADER);
  fragment = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vertex, 1, &vertexShaderSource, NULL);
  glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

  glCompileShader(vertex);
  checkShaderErrors(vertex, ErrorType::COMPILE);
  glCompileShader(fragment);
  checkShaderErrors(fragment, ErrorType::COMPILE);

  _program = glCreateProgram();
  glAttachShader(_program, vertex);
  glAttachShader(_program, fragment);
  glLinkProgram(_program);
  checkShaderErrors(_program, ErrorType::LINK);

  glDeleteShader(vertex);
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