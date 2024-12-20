#pragma once

#include "runtime/framework/object/gameObject.h"

#include <vector>

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