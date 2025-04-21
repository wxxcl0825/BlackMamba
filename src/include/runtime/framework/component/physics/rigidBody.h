#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

#include "runtime/engine.h"
#include "runtime/framework/system/physicalSystem.h"
#include "runtime/framework/system/jolt/utils.h"
#include "runtime/framework/component/component.h"

class RigidBodyComponent : public Component {
public:
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, JPH::ShapeRefC shape)
      : _motionType(motionType), _layer(layer), _shape(shape) {};
  
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, uint32_t x, uint32_t y, uint32_t z, float density);

  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float density);

  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float halfHeight, float density);

  ~RigidBodyComponent() override{Engine::getEngine()->getPhysicalSystem()->unregisterComponent(_id);};


  uint32_t getId() const { return _id; }
  float getDensity() const {return _density;}
  float getLiftCoefficient() const {return _liftCoefficient;}
  JPH::EMotionType getMotionType() const { return _motionType; }
  JPH::ObjectLayer getLayer() const { return _layer; }
  JPH::ShapeRefC getShape() const { return _shape; }
  glm::vec3 getForce() const { return _force; }
  glm::vec3 getTorque() const { return _torque; }
  glm::vec3 getLinearVelocity() const { return _linearVelocity; }
  float getMaxLinearVelocity() const { return _maxLinearVelocity; }
  float getMaxAngularVelocity() const { return _maxAngularVelocity; }
  bool isCollide() const { return _isCollide; }

  void setId(uint32_t id) { _id = id; }
  void setDensity(float density) { _density = density; }
  void setLiftCoefficient(float liftCoefficient) { _liftCoefficient = liftCoefficient; }
  void setMotionType(JPH::EMotionType type) { _motionType = type; }
  void setLayer(JPH::ObjectLayer layer) { this->_layer = layer; }
  void setShape(JPH::Shape* shape) { this->_shape = shape; }
  void setForce(glm::vec3 force) { _force = force; }
  void setTorque(glm::vec3 torque) { _torque = torque; }
  void setLinearVelocity(glm::vec3 linearVelocity) { _linearVelocity = linearVelocity; }
  void setMaxLinearVelocity(float maxLinearVelocity) { _maxLinearVelocity = maxLinearVelocity; }
  void setMaxAngularVelocity(float maxAngularVelocity) { _maxAngularVelocity = maxAngularVelocity; }
  void setCollide(bool isCollide) { _isCollide = isCollide; }

private:
  uint32_t _id {0xffffffff};

  float _density {1.0f};
  float _liftCoefficient {0.5f};
  JPH::EMotionType _motionType = JPH::EMotionType::Static;
  JPH::ObjectLayer _layer       = Layers::STATIC;
  JPH::ShapeRefC _shape {nullptr};

  glm::vec3 _force {0.0f, 0.0f, 0.0f};
  glm::vec3 _torque {0.0f, 0.0f, 0.0f};
  glm::vec3 _linearVelocity {0.0f, 0.0f, 0.0f};
  float _maxLinearVelocity {500.0f};
  float _maxAngularVelocity {0.25f * JPH::JPH_PI};

  bool _isCollide {false};
};