#include "game/material/transparentMaterial.h"
#include "game/game.h"
#include "runtime/resource/resourceManager.h"

TransparentMaterial::TransparentMaterial() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader(
      "assets/shaders/transparent/transparent.vert",
      "assets/shaders/transparent/transparent.frag");

  _blend = true;
  _depthWrite = false;
}

void TransparentMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _shader->setUniform("model", info.modelInfo.model);
  _shader->setUniform("view", info.cameraInfo.view);
  _shader->setUniform("projection", info.cameraInfo.project);
  _shader->setUniform("normalMatrix", glm::mat3(glm::transpose(
                                          glm::inverse(info.modelInfo.model))));

  _diffuse->bind();
  _shader->setUniform("diffuse", (int)_diffuse->getUnit());
  _opacity->bind();
  _shader->setUniform("opacity", (int)_opacity->getUnit());
  _shader->setUniform("transparency", _transparency);

  _shader->setUniform("ambientLight.color", info.lightInfo.ambientLights[0].color);
}