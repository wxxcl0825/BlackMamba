#pragma once

#include "common/common.h"
#include "runtime/framework/system/renderSystem.h"
#include "runtime/resource/shader/shader.h"

class Material {
public:
  virtual ~Material() = 0;

  virtual void apply(const RenderInfo &info) = 0;

  void finish() {
    if (_shader)
      _shader->end();
  };

protected:
  Shader *_shader = nullptr;

  bool _depthTest = true;
  GLenum _depthFunc = GL_LEQUAL;
  bool _depthWrite = true;

  unsigned int _zPass = GL_KEEP;
};

inline Material::~Material() {}
inline void apply(const RenderInfo &info) {}