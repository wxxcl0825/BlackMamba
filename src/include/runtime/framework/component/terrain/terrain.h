#pragma once

#include "runtime/framework/component/component.h"

class TerrainComponent : public Component {
public:
  TerrainComponent(float rez) : _rez(rez) {}
  ~TerrainComponent() override {}

  int getRez() const { return _rez; }

private:
  int _rez{20};
};