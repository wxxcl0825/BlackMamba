#include "runtime/framework/system/renderSystem.h"

#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/mesh/mesh.h"

void RenderSystem::dispatch(GameObject *object) {
  if (object->getComponent<Mesh>())
    _meshes.push_back(object);
  if (object->getComponent<Light>())
    _lights.push_back(object);
  if (object->getComponent<Camera>())
    _cameras.push_back(object);
}

void RenderSystem::tick() { clear(); }

void RenderSystem::clear() {
  _meshes.clear();
  _lights.clear();
  _cameras.clear();
}