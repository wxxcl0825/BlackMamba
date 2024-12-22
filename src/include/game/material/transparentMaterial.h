#pragma once

#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class TransparentMaterial : public Material {
public:
  TransparentMaterial();
  TransparentMaterial(const TransparentMaterial &other) : Material(other) {
    _diffuse = other._diffuse;
    _opacity = other._opacity;
    _transparency = other._transparency;
  }

  ~TransparentMaterial() override{};

  TransparentMaterial *clone() const override {
    return new TransparentMaterial(*this);
  }

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }
  void setOpacity(Texture *opacity) {
    _opacity = opacity;
    _opacity->setUnit(1);
  }
  void setTransparency(float transparency) { _transparency = transparency; }

private:
  Texture *_diffuse{nullptr};
  Texture *_opacity{nullptr};
  float _transparency{0.5f};
};