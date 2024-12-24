#pragma once

#include "game/game.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/resourceManager.h"
#include "runtime/resource/texture/texture.h"

class PhongMaterial : public Material {
public:
  PhongMaterial();
  PhongMaterial(const PhongMaterial &other) : Material(other) {
    _diffuse = other._diffuse;
    _shiness = other._shiness;
  }
  PhongMaterial(const Material &other) : Material(other) {
    ResourceManager *resourceManager = Game::getGame()->getEngine()->getResourceManager();
    _shader = resourceManager->loadShader(
      "assets/shaders/phong/phong.vert", "assets/shaders/phong/phong.frag");
    _diffuse = nullptr;
    _shiness = 32.0f;
  }

  PhongMaterial* clone() const override { return new PhongMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }
  void setShiness(float shiness) { _shiness = shiness; }

private:
  Texture *_diffuse{nullptr};
  float _shiness{32.0f};
};