#include "game/material/terrainMaterial.h"

#include "game/game.h"
#include "runtime/resource/resourceManager.h"

TerrainMaterial::TerrainMaterial() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader("assets/shaders/terrain/terrain.vert",
                                        "assets/shaders/terrain/terrain.tesc",
                                        "assets/shaders/terrain/terrain.tese",
                                        "assets/shaders/terrain/terrain.frag");
}

void TerrainMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _heightMap->bind();
  _shader->setUniform("heightMap", (int)_heightMap->getUnit());
  _shader->setUniform("model", info.modelInfo.model);
  _shader->setUniform("view", info.cameraInfo.view);
  _shader->setUniform("projection", info.cameraInfo.project);

  _diffuse->bind();
  _shader->setUniform("diffuse", (int)_diffuse->getUnit());
}