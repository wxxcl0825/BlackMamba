#pragma once

#include "game/material/phongMaterial.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/texture/texture.h"

class TerrainMaterial : public PhongMaterial {
public:
  TerrainMaterial() : PhongMaterial() { prepareShader(); }
  TerrainMaterial(const TerrainMaterial &other) : PhongMaterial(other) {
    _heightMap = other._heightMap;
  }
  TerrainMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    prepareShader();
    _heightMap = nullptr;
  }
  TerrainMaterial(const Material &other) : PhongMaterial(other) {
    prepareShader();
    _heightMap = nullptr;
  }

  ~TerrainMaterial() override {}

  TerrainMaterial *clone() const override { return new TerrainMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override {
    PhongMaterial::setDiffuse(diffuse);
  }
  void setHeightMap(Texture *heightMap) {
    _heightMap = heightMap;
    _heightMap->setUnit(1);
  }

private:
  Texture *_heightMap{nullptr};

  void prepareShader();
};