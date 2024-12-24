#include "game/material/phongMaterial.h"

#include "game/game.h"
#include "runtime/resource/resourceManager.h"
#include <string>
PhongMaterial::PhongMaterial() {
  ResourceManager *resourceManager = Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader(
      "assets/shaders/phong/phong.vert", "assets/shaders/phong/phong.frag");
}

void PhongMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _shader->setUniform("model", info.modelInfo.model);
  _shader->setUniform("view", info.cameraInfo.view);
  _shader->setUniform("projection", info.cameraInfo.project);
  _shader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(info.modelInfo.model))));

  _diffuse->bind();
  _shader->setUniform("diffuse", (int)_diffuse->getUnit());
  _shader->setUniform("shiness", _shiness);
  _shader->setUniform("cameraPosition", info.cameraInfo.position);

  _shader->setUniform("numDirectionalLight", int(info.lightInfo.directionalLights.size()));
  for (int i = 0; i < info.lightInfo.directionalLights.size(); i++) {
    auto &dl = info.lightInfo.directionalLights[i];
    std::string prefix = "directionalLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".color", dl.color);
    _shader->setUniform(prefix + ".direction", dl.direction);
    _shader->setUniform(prefix + ".specularIntensity", dl.specularIntensity);
  }

  _shader->setUniform("numPointLight", int(info.lightInfo.pointLights.size()));
  for (int i = 0; i < info.lightInfo.pointLights.size(); i++) {
    auto &pl = info.lightInfo.pointLights[i];
    std::string prefix = "pointLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".position", pl.position);
    _shader->setUniform(prefix + ".color", pl.color);
    _shader->setUniform(prefix + ".specularIntensity", pl.specularIntensity);
    _shader->setUniform(prefix + ".k2", pl.k2);
    _shader->setUniform(prefix + ".k1", pl.k1);
    _shader->setUniform(prefix + ".kc", pl.kc);
  }

  _shader->setUniform("numSpotLight", int(info.lightInfo.spotLights.size()));
  for (int i = 0; i < info.lightInfo.spotLights.size(); i++) {
    auto &sl = info.lightInfo.spotLights[i];
    std::string prefix = "spotLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".position", sl.position);
    _shader->setUniform(prefix + ".color", sl.color);
    _shader->setUniform(prefix + ".direction", sl.direction);
    _shader->setUniform(prefix + ".specularIntensity", sl.specularIntensity);
    _shader->setUniform(prefix + ".inner", sl.inner);
    _shader->setUniform(prefix + ".outer", sl.outer);
  }

  _shader->setUniform("ambientLight.color", info.lightInfo.ambientLights[0].color);

  _shader->setUniform("useAlphaMap", .0f);
}