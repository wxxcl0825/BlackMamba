#pragma once

#include "common/common.h"
#include "runtime/framework/system/renderSystem.h"
#include "runtime/resource/shader/shader.h"
#include "runtime/resource/texture/texture.h"

class Material {
public:
  virtual ~Material() = 0;

  Material() = default;
  Material(const Material& other) {
    _shader = other._shader;
    _depthTest = other._depthTest;
    _depthFunc = other._depthFunc;
    _depthWrite = other._depthWrite;
    _blend = other._blend;
    _sFactor = other._sFactor;
    _dFactor = other._dFactor;
  }

  virtual Material *clone() const = 0;
  virtual void apply(const RenderInfo &info) = 0;

  bool getDepthTest() const { return _depthTest; }
  GLenum getDepthFunc() const { return _depthFunc; }
  bool getDepthWrite() const { return _depthWrite; }
  bool getBlend() const { return _blend; }
  unsigned int getSFactor() const { return _sFactor; }
  unsigned int getDFactor() const { return _dFactor; }

  virtual void setDiffuse(Texture *diffuse){};

  void finish() {
    if (_shader)
      _shader->end();
  };

protected:
  Shader *_shader = nullptr;

  // Depth test
  bool _depthTest = true;
  GLenum _depthFunc = GL_LESS;
  bool _depthWrite = true;

  // Color blending
  bool _blend = false;
  unsigned int _sFactor = GL_SRC_ALPHA;
  unsigned int _dFactor = GL_ONE_MINUS_SRC_ALPHA;
};

inline Material::~Material() {}
inline void apply(const RenderInfo &info) {}