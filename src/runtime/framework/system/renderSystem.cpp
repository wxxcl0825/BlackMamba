#include "runtime/framework/system/renderSystem.h"

#include "common/macro.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/terrain/terrain.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/material/material.h"

void RenderSystem::dispatch(GameObject *object) {
  if (object->getComponent<MeshComponent>())
    _meshes.push_back(object);
  if (object->getComponent<LightComponent>())
    _lights.push_back(object);
  if (object->getComponent<CameraComponent>())
    _cameras.push_back(object);
  if (object->getComponent<TerrainComponent>())
    _meshes.push_back(object);
}

void RenderSystem::tick() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GameObject *mainCamera = nullptr;
  for (auto camera : _cameras) {
    if (camera->getComponent<CameraComponent>()->isMain()) {
      mainCamera = camera;
      break;
    }
  }
  if (!mainCamera)
    Err("No main camera found!");
  std::shared_ptr<CameraComponent> mainCameraComp = mainCamera->getComponent<CameraComponent>();
  std::shared_ptr<TransformComponent> mainCameraTransfrom = mainCamera->getComponent<TransformComponent>();
  glm::vec3 mainCameraPosition =  mainCameraTransfrom->getModel() * glm::vec4(mainCameraTransfrom->getPosition(), 1.0f);
  CameraInfo cameraInfo{
      .position = mainCameraPosition,
      .view = mainCameraComp->getView(mainCameraPosition),
      .project = mainCameraComp->getProjection()};

  LightInfo lightInfo;
  for (auto light : _lights) {
    std::shared_ptr<LightComponent> lightComp = light->getComponent<LightComponent>();
    switch (lightComp->getType()) {
    case LightComponent::Type::Directional:
      lightInfo.directionalLights.emplace_back(DirectionalLight{
          .color = lightComp->getColor(),
          .direction = lightComp->getDirection(),
          .specularIntensity = lightComp->getSpecularIntensity()});
      break;
    case LightComponent::Type::Point:
      lightInfo.pointLights.emplace_back(PointLight{
          .position = light->getComponent<TransformComponent>()->getPosition(),
          .color = lightComp->getColor(),
          .specularIntensity = lightComp->getSpecularIntensity(),
          .k2 = lightComp->getK2(),
          .k1 = lightComp->getK1(),
          .kc = lightComp->getKc()});
      break;
    case LightComponent::Type::Spot:
      lightInfo.spotLights.emplace_back(
          SpotLight{.position = light->getComponent<TransformComponent>()->getPosition(),
                    .color = lightComp->getColor(),
                    .direction = lightComp->getDirection(),
                    .specularIntensity = lightComp->getSpecularIntensity(),
                    .inner = lightComp->getInner(),
                    .outer = lightComp->getOuter()});
      break;
    case LightComponent::Type::Ambient:
      lightInfo.ambientLights.emplace_back(
          AmbientLight{.color = lightComp->getColor()});
      break;
    default:
      Log("Unknow light type. Abort.");
      break;
    }
  }

  for (auto mesh : _meshes) {
    std::shared_ptr<MeshComponent> meshComp = mesh->getComponent<MeshComponent>();
    Geometry *geometry = meshComp->getGeometry();
    Material *material = meshComp->getMaterial();
    ModelInfo modelInfo{.model = mesh->getComponent<TransformComponent>()->getModel()};
    material->apply(RenderInfo{.modelInfo = modelInfo,
                               .cameraInfo = cameraInfo,
                               .lightInfo = lightInfo});
    GL_CALL(glBindVertexArray(geometry->getVao()));
    if (mesh->getComponent<TerrainComponent>()) {
      int rez = mesh->getComponent<TerrainComponent>()->getRez();
      GL_CALL(glPatchParameteri(GL_PATCH_VERTICES, 4));
      GL_CALL(glDrawArrays(GL_PATCHES, 0, 4 * rez * rez));
    } else { 
      GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getNumIndices(),
                           GL_UNSIGNED_INT, 0));
    }
    material->finish();
  }
  clear();
}

void RenderSystem::clear() {
  _meshes.clear();
  _lights.clear();
  _cameras.clear();
}