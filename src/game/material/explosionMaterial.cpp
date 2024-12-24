#include "game/material/explosionMaterial.h"
#include "game/game.h"
#include "game/material/phongMaterial.h"
#include "runtime/resource/resourceManager.h"

void ExplosionMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

  _shader->setUniform("time", _time);
}

void ExplosionMaterial::prepareShader() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader("assets/shaders/phong/phong.vert",
                                        "assets/shaders/effect/explode.geom",
                                        "assets/shaders/phong/phong.frag");
}