#include "game/material/skyboxMaterial.h"
#include "game/game.h"
#include "runtime/resource/resourceManager.h"

SkyboxMaterial::SkyboxMaterial() {
  ResourceManager* resourceManager = Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader(
      "assets/shaders/skybox/skybox.vert", "assets/shaders/skybox/skybox.frag");

  _depthWrite = false;
}

void SkyboxMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _shader->setUniform("model",info.modelInfo.model);
  _shader->setUniform("view",info.cameraInfo.view);
  _shader->setUniform("projection",info.cameraInfo.project);

  _diffuse->bind();
  _shader->setUniform("sampler", (int)_diffuse->getUnit());
}