#include "runtime/framework/system/renderSystem.h"

#include "common/macro.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"

void RenderSystem::dispatch(GameObject *object) {
  if (object->getComponent<Mesh>())
    _meshes.push_back(object);
  if (object->getComponent<Light>())
    _lights.push_back(object);
  if (object->getComponent<Camera>())
    _cameras.push_back(object);
}

void RenderSystem::tick() {
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
  clear();
}

void RenderSystem::clear() {
  _meshes.clear();
  _lights.clear();
  _cameras.clear();
}