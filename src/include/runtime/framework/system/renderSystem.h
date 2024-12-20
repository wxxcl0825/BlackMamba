#pragma once

#include "runtime/framework/component/light/light.h"
#include "runtime/framework/object/gameObject.h"

#include "common/common.h"
#include <vector>

struct ModelInfo {
  glm::mat4 model;
};

struct CameraInfo {
  glm::vec3 position;
  glm::mat4 view;
  glm::mat4 project;
};

struct LightInfo {
  std::vector<DirectionalLight> directionalLights{};
  std::vector<PointLight> pointLights{};
  std::vector<SpotLight> spotLights{};
  std::vector<AmbientLight> ambientLights{};
};

struct RenderInfo {
  ModelInfo &modelInfo;
  CameraInfo &cameraInfo;
  LightInfo &lightInfo;
};

class RenderSystem {
public:
  RenderSystem() = default;
  ~RenderSystem() = default;

  void dispatch(GameObject *object);
  void tick();

private:
  std::vector<GameObject *> _meshes, _lights, _cameras;

  void clear();
};