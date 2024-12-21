#include "game/entity/skybox.h"

#include "game/material/skyboxMaterial.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/resourceManager.h"

Skybox::Skybox(const std::vector<std::string> &paths) {
  ResourceManager *resourceManager = ResourceManager::getResourceManager();
  _skybox = new GameObject();
  Geometry *box = resourceManager->createBoxGeometry(1.0f);
  SkyboxMaterial *material = new SkyboxMaterial();
  material->setDiffuse(resourceManager->loadTexture(paths));
  _skybox->addComponent(std::make_shared<MeshComponent>(box, material));
}

void Skybox::bind(GameObject *camera) {
  _skybox->setComponent(camera->getComponent<TransformComponent>());
}