#pragma once

#include "game/material/terrainMaterial.h"
#include "runtime/framework/object/gameObject.h"
class Terrain {
public:
  Terrain(float width, float height, int rez, int repeat, TerrainMaterial *material);
  ~Terrain();

  GameObject *getTerrain() const { return _terrain; }

private:
  GameObject *_terrain{nullptr};
};