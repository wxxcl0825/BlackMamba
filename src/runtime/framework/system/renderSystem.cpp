#include "runtime/framework/system/renderSystem.h"

#include "common/macro.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/material/material.h"
#include "runtime/resource/shader/shader.h"

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
  // TODO: gather information
  for (auto mesh : _meshes) {
    Mesh *meshComp = mesh->getComponent<Mesh>();
    Geometry *geometry = meshComp->getGeometry();
    Material *material = meshComp->getMaterial();
    // TODO: call material
    GL_CALL(glBindVertexArray(geometry->getVao()));
    GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getNumIndices(), GL_UNSIGNED_INT, 0));
    // TODO: close shader
  }
  clear();
}

void RenderSystem::clear() {
  _meshes.clear();
  _lights.clear();
  _cameras.clear();
}