#pragma once

#include "common/common.h"
#include "runtime/resource/shader/shader.h"
#include "runtime/framework/system/renderSystem.h"

class Material{
public:
    virtual ~Material() = 0;

    virtual void apply(const RenderInfo &info) = 0;
    virtual void finish() = 0;

protected:
    Shader *_shader = nullptr;

    bool _depthTest = true;
    GLenum _depthFunc = GL_LEQUAL;
    bool _depthWrite = true;

    unsigned int _zPass = GL_KEEP;
};

inline Material::~Material() {}
inline void apply(const RenderInfo &info) {}
inline void finish() {}