#pragma once

#include "common/common.h"
#include "runtime/resource/material/material.h"

class WhiteMaterial : public Material {
public:
    WhiteMaterial();
    ~WhiteMaterial() override {};
    void setColor(const glm::vec3& color);

    void apply(const RenderInfo &info) override;

private:
    glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f);
};