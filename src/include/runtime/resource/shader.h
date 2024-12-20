#pragma once

#include "common/common.h"
#include "common/macro.h"
#include <string>
#include <fstream>
#include <sstream>

class Shader{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;

    unsigned int getProgramID() const { return _programID; }

    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, glm::vec3 value) const;
    void setUniform(const std::string& name, glm::mat3 value) const;
    void setUniform(const std::string& name, glm::mat4 value) const;

private:
    unsigned int _programID{0};
    std::string _vertexPath;
    std::string _fragmentPath;
};