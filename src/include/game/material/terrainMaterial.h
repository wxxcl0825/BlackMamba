#pragma once

#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class TerrainMaterial : public Material {
public:
  TerrainMaterial();

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }
  void setHeightMap(Texture *heightMap) {
    _heightMap = heightMap;
    _heightMap->setUnit(1);
  }

private:
  Texture *_diffuse{nullptr};
  Texture *_heightMap{nullptr};
};