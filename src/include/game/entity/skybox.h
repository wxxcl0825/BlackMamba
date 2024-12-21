#pragma once

#include "runtime/framework/object/gameObject.h"
#include <string>
#include <vector>

class Skybox {
public:
  Skybox(const std::vector<std::string> &paths);

  void bind(GameObject *camera);

  GameObject *getSkybox() const { return _skybox; }

private:
  GameObject *_skybox{nullptr};
};