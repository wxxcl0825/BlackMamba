#pragma once

#include "runtime/framework/system/renderSystem.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class SkyboxMaterial : public Material {
public:
  SkyboxMaterial();

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) { _diffuse = diffuse; }

private:
  Texture *_diffuse{nullptr};
};