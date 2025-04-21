#pragma once

#include "runtime/framework/system/renderSystem.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class SkyboxMaterial : public Material {
public:
  SkyboxMaterial();
  SkyboxMaterial(const SkyboxMaterial &other) : Material(other) {
    _diffuse = other._diffuse;
  }

  SkyboxMaterial* clone() const override { return new SkyboxMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }

private:
  Texture *_diffuse{nullptr};
};