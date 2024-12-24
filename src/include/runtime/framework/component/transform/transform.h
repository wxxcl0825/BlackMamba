#pragma once

#include "common/common.h"
#include "common/macro.h"
#include "glm/fwd.hpp"
#include "runtime/framework/component/component.h"

class TransformComponent : public Component {
public:
  TransformComponent() = default;
  ~TransformComponent() override{};

  glm::mat4 getModel() const;
  glm::vec3 getPositionLocal() const { return _position; }
  glm::vec3 getPositionWorld() const { return getModel() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); }
  glm::vec3 getForwardVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_forwardVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }
  glm::vec3 getUpVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_upVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }
  glm::vec3 getRightVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_rightVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }

  glm::vec3 getAngle() const { return _angle; }
  glm::vec3 getScale() const { return _scale; }
  glm::mat4 getParentModel() const { return _parentModel; }

  void setPositionLocal(const glm::vec3 &position) { _position = position; }
  void setAngle(const glm::vec3 &angle) { _angle = angle; }
  void setScale(const glm::vec3 &scale) { _scale = scale; }
  void setParentModel(const glm::mat4 &model) { _parentModel = model; }

private:
  glm::vec3 _position{glm::zero<float>()};
  glm::vec3 _angle{glm::zero<float>()};
  glm::vec3 _scale{glm::one<float>()};

  glm::vec3 _forwardVec{0.0f, 0.0f, 1.0f};
  glm::vec3 _upVec{0.0f, 1.0f, 0.0f};
  glm::vec3 _rightVec{-1.0f, 0.0f, 0.0f};

  glm::mat4 _parentModel{glm::one<float>()};
};