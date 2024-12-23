#pragma once

#include "game/material/phongMaterial.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class TransparentMaterial : public PhongMaterial {
public:
  TransparentMaterial() : PhongMaterial() {
    _blend = true;
    _depthWrite = false;
  };
  TransparentMaterial(const TransparentMaterial &other) : PhongMaterial(other) {
    _alphaMap = other._alphaMap;
  }
  TransparentMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    _blend = true;
    _depthWrite = false;
    _alphaMap = nullptr;
  }
  TransparentMaterial(const Material &other) : PhongMaterial(other) {
    _blend = true;
    _depthWrite = false;
    _alphaMap = nullptr;
  }

  ~TransparentMaterial() override {}

  TransparentMaterial *clone() const override {
    return new TransparentMaterial(*this);
  }

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override {
    PhongMaterial::setDiffuse(diffuse);
  }
  void setAlphaMap(Texture *alphaMap) {
    _alphaMap = alphaMap;
    _alphaMap->setUnit(1);
  }

private:
  Texture *_alphaMap{nullptr};
};