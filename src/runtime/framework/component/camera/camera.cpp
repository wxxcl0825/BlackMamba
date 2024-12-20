#include "runtime/framework/component/camera/camera.h"
#include "common/macro.h"

Camera::Camera(float left, float right, float top, float bottom, float near,
               float far) {
  _type = Type::Ortho;
  _left = left, _right = right, _top = top, _bottom = bottom, _near = near,
  _far = far;
}

Camera::Camera(float fovy, float aspect, float near, float far) {
  _type = Type::Perspective;
  _fovy = fovy, _aspect = aspect, _near = near, _far = far;
}

glm::mat4 Camera::getView(glm::vec3 position) const {
  glm::vec3 front = glm::cross(_upVec, _rightVec);
  return glm::lookAt(position, position + front, _upVec);
}

glm::mat4 Camera::getProjection() const {
  if (_type == Type::Ortho)
    return glm::ortho(_left, _right, _bottom, _top);
  else if (_type == Type::Perspective)
    return glm::perspective(glm::radians(_fovy), _aspect, _near, _far);
  Log("Unknown camera type!");
  return glm::mat4(glm::one<float>());
}