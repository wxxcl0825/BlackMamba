#include "runtime/resource/shader.h"
#include "common/macro.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try{
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }catch(std::ifstream::failure e){
        Err("Failed to read shader file");
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        Err("Vertex shader compilation failed: %s", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        Err("Fragment shader compilation failed: %s", infoLog);
    }

    _programID = glCreateProgram();
    glAttachShader(_programID, vertex);
    glAttachShader(_programID, fragment);
    glLinkProgram(_programID);
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        Err("Shader program linking failed: %s", infoLog);
    }

    GL_CALL(glDeleteShader(vertex));
    GL_CALL(glDeleteShader(fragment));
}

Shader::~Shader(){
    GL_CALL(glDeleteProgram(_programID));
}

void Shader::use() const{
    GL_CALL(glUseProgram(_programID));
}

void Shader::setUniform(const std::string& name, float value) const{
    GL_CALL(glUniform1f(glGetUniformLocation(_programID, name.c_str()), value));
}

void Shader::setUniform(const std::string& name, int value) const{
    GL_CALL(glUniform1i(glGetUniformLocation(_programID, name.c_str()), value));
}

void Shader::setUniform(const std::string& name, glm::vec3 value) const{
    GL_CALL(glUniform3fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]));
}

void Shader::setUniform(const std::string& name, glm::mat3 value) const{
    GL_CALL(glUniformMatrix3fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &value[0][0]));
}

void Shader::setUniform(const std::string& name, glm::mat4 value) const{
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &value[0][0]));
}