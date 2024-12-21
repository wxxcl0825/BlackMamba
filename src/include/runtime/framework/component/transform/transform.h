#pragma once

#include "common/common.h"
#include "runtime/framework/component/component.h"

class TransformComponent : public Component {
public:
  TransformComponent() = default;
  ~TransformComponent() override{};

  glm::mat4 getModel() const;
  glm::vec3 getPosition() const { return _position; }

  void setPosition(const glm::vec3 &position) { _position = position; }
  void setAngle(const glm::vec3 &angle) { _angle = angle; }
  void setScale(const glm::vec3 &scale) { _scale = scale; }
  void setParentModel(const glm::mat4 &model) { _parentModel = model; }

private:
  glm::vec3 _position{glm::zero<float>()};
  glm::vec3 _angle{glm::zero<float>()};
  glm::vec3 _scale{glm::one<float>()};

  glm::mat4 _parentModel{glm::one<float>()};
};