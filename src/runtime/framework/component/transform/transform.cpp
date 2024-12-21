#include "runtime/framework/component/transform/transform.h"

glm::mat4 TransformComponent::getModel() const {
  glm::mat4 model{1.0f};
  model = glm::scale(model, _scale);
  model = glm::rotate(model, glm::radians(_angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(_angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(_angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
  return _parentModel * glm::translate(glm::mat4(1.0f), _position) * model;
}