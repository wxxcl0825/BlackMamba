#include "runtime/framework/system/renderSystem.h"

#include "common/macro.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/material/material.h"

void RenderSystem::dispatch(GameObject *object) {
  if (object->getComponent<Mesh>())
    _meshes.push_back(object);
  if (object->getComponent<Light>())
    _lights.push_back(object);
  if (object->getComponent<Camera>())
    _cameras.push_back(object);
}

void RenderSystem::tick() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GameObject *mainCamera = nullptr;
  for (auto camera : _cameras) {
    if (camera->getComponent<Camera>()->isMain()) {
      mainCamera = camera;
      break;
    }
  }
  if (!mainCamera)
    Err("No main camera found!");
  Camera *mainCameraComp = mainCamera->getComponent<Camera>();
  Transform *mainCameraTransfrom = mainCamera->getComponent<Transform>();
  CameraInfo cameraInfo{
      .position = mainCameraTransfrom->getPosition(),
      .view = mainCameraComp->getView(mainCameraTransfrom->getPosition()),
      .project = mainCameraComp->getProjection()};

  LightInfo lightInfo;
  for (auto light : _lights) {
    Light *lightComp = light->getComponent<Light>();
    switch (lightComp->getType()) {
    case Light::Type::Directional:
      lightInfo.directionalLights.emplace_back(DirectionalLight{
          .color = lightComp->getColor(),
          .direction = lightComp->getDirection(),
          .specularIntensity = lightComp->getSpecularIntensity()});
      break;
    case Light::Type::Point:
      lightInfo.pointLights.emplace_back(PointLight{
          .position = light->getComponent<Transform>()->getPosition(),
          .color = lightComp->getColor(),
          .specularIntensity = lightComp->getSpecularIntensity(),
          .k2 = lightComp->getK2(),
          .k1 = lightComp->getK1(),
          .kc = lightComp->getKc()});
      break;
    case Light::Type::Spot:
      lightInfo.spotLights.emplace_back(
          SpotLight{.position = light->getComponent<Transform>()->getPosition(),
                    .color = lightComp->getColor(),
                    .direction = lightComp->getDirection(),
                    .specularIntensity = lightComp->getSpecularIntensity(),
                    .inner = lightComp->getInner(),
                    .outer = lightComp->getOuter()});
      break;
    case Light::Type::Ambient:
      lightInfo.ambientLights.emplace_back(
          AmbientLight{.color = lightComp->getColor()});
      break;
    default:
      Log("Unknow light type. Abort.");
      break;
    }
  }

  for (auto mesh : _meshes) {
    Mesh *meshComp = mesh->getComponent<Mesh>();
    Geometry *geometry = meshComp->getGeometry();
    Material *material = meshComp->getMaterial();
    ModelInfo modelInfo{.model = mesh->getComponent<Transform>()->getModel()};
    material->apply(RenderInfo{.modelInfo = modelInfo,
                               .cameraInfo = cameraInfo,
                               .lightInfo = lightInfo});
    GL_CALL(glBindVertexArray(geometry->getVao()));
    GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getNumIndices(),
                           GL_UNSIGNED_INT, 0));
    material->finish();
  }
  clear();
}

void RenderSystem::clear() {
  _meshes.clear();
  _lights.clear();
  _cameras.clear();
}