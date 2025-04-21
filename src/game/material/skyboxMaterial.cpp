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

  glm::mat4 model = info.modelInfo.model;
  model[0] = glm::vec4(1.0f, 0.0f, 0.0f, model[0].w);
  model[1] = glm::vec4(0.0f, 1.0f, 0.0f, model[1].w);
  model[2] = glm::vec4(0.0f, 0.0f, 1.0f, model[2].w);
  _shader->setUniform("model", model);
  _shader->setUniform("view",info.cameraInfo.view);
  _shader->setUniform("projection",info.cameraInfo.project);

  _diffuse->bind();
  _shader->setUniform("sampler", (int)_diffuse->getUnit());
}