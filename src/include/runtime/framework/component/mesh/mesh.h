#pragma once

#include "runtime/framework/component/component.h"
#include "runtime/resource/geometry/geometry.h"
#include "runtime/resource/material/material.h"

class MeshComponent : public Component {
public:
  MeshComponent(Geometry *geometry, Material *material)
      : _geometry(geometry), _material(material) {}
  ~MeshComponent() override {}

  Geometry *getGeometry() const { return _geometry; }
  Material *getMaterial() const { return _material; }

private:
  Geometry *_geometry;
  Material *_material;
};