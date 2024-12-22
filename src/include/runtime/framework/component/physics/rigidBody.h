#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <cstdint>

#include "glm/fwd.hpp"
#include "runtime/engine.h"
#include "runtime/framework/system/physicalSystem.h"
#include "runtime/framework/system/jolt/utils.h"
#include "runtime/framework/component/component.h"

class RigidBodyComponent : public Component {
public:
  RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, JPH::ShapeRefC shape)
      : _motion_type(motion_type), _layer(layer), _shape(shape) { _id = Engine::getEngine()->getPhysicalSystem()->registerComponent(); };
  
  RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, uint32_t x, uint32_t y, uint32_t z)
      : _motion_type(motion_type), _layer(layer)
  { JPH::BoxShapeSettings body_shape_settings(JPH::Vec3(0.5 * x, 0.5 * y, 0.5 * z));
    body_shape_settings.mConvexRadius = 0.01;
    body_shape_settings.SetDensity(_density);
    body_shape_settings.SetEmbedded();
    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
    _id = Engine::getEngine()->getPhysicalSystem()->registerComponent(); };

  RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, float radius)
      : _motion_type(motion_type), _layer(layer)
  { JPH::SphereShapeSettings body_shape_settings(radius);
    body_shape_settings.SetDensity(_density);
    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
    _id = Engine::getEngine()->getPhysicalSystem()->registerComponent(); };

  RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, float radius, float half_height)
      : _motion_type(motion_type), _layer(layer)
  { JPH::CapsuleShapeSettings body_shape_settings(half_height,radius);
    body_shape_settings.SetDensity(_density);
    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
    _id = Engine::getEngine()->getPhysicalSystem()->registerComponent(); };

  ~RigidBodyComponent() override{Engine::getEngine()->getPhysicalSystem()->unregisterComponent(_id);};

  JPH::EMotionType getMotionType() const { return _motion_type; }
  JPH::ObjectLayer getLayer() const { return _layer; }
  JPH::ShapeRefC getShape() const { return _shape; }
  uint32_t getRigidBodyId() const { return _rigidbody_id; }
  float getDensity() const {return _density;}
  glm::vec3 getForce() const { return _force; }

  void setMotionType(JPH::EMotionType type) { _motion_type = type; }
  void setLayer(JPH::ObjectLayer layer) { this->_layer = layer; }
  void setShape(JPH::Shape* shape) { this->_shape = shape; }
  void setRigidBodyId(uint32_t id) { _rigidbody_id = id; }
  void setDensity(float density) { _density = density; }
  void setForce(glm::vec3 force) { _force = force; }

private:
  JPH::EMotionType _motion_type = JPH::EMotionType::Static;
  JPH::ObjectLayer _layer       = Layers::NON_MOVING;
  JPH::ShapeRefC _shape {nullptr};
  uint32_t _id;
  uint32_t _rigidbody_id {0xffffffff};

  float _density {10.0f};
  glm::vec3 _force {0.0f, 0.0f, 0.0f};
};