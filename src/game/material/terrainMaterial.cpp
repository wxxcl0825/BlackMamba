#include "game/material/terrainMaterial.h"

#include "game/game.h"
#include "runtime/resource/resourceManager.h"

void TerrainMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

  _heightMap->bind();
  _shader->setUniform("heightMap", (int)_heightMap->getUnit());
}

void TerrainMaterial::prepareShader() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader("assets/shaders/terrain/terrain.vert",
                                        "assets/shaders/terrain/terrain.tesc",
                                        "assets/shaders/terrain/terrain.tese",
                                        "assets/shaders/phong/phong.frag");
}