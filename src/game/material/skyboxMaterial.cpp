#include "game/material/skyboxMaterial.h"
#include "runtime/resource/resourceManager.h"

SkyboxMaterial::SkyboxMaterial() {
  _shader = ResourceManager::getResourceManager()->loadShader(
      "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
}

void SkyboxMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _shader->setUniform("model",info.modelInfo.model);
  _shader->setUniform("view",info.cameraInfo.view);
  _shader->setUniform("projection",info.cameraInfo.project);

  _diffuse->bind();
  _shader->setUniform("sampler", (int)_diffuse->getUnit());
}