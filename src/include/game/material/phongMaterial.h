#pragma once

#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class PhongMaterial : public Material {
public:
  PhongMaterial();

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }
  void setShiness(float shiness) { _shiness = shiness; }

private:
  Texture *_diffuse{nullptr};
  float _shiness{32.0f};
};