#pragma once

#include "glm/fwd.hpp"
#include "runtime/framework/component/component.h"

class CameraComponent : public Component {
public:
  enum class Type { Ortho, Perspective, Invalid };

  CameraComponent(float left, float right, float top, float bottom, float near,
         float far);
  CameraComponent(float fovy, float aspect, float near, float far);

  ~CameraComponent() override{};

  bool isMain() const { return _isMain; }
  void pick() { _isMain = true; }
  void put() { _isMain = false; }

  glm::mat4 getView(glm::vec3 position) const;
  glm::mat4 getProjection() const;

  const glm::vec3 &getUpVec() const { return _upVec; }
  const glm::vec3 &getRightVec() const { return _rightVec; }

  void setUpVec(const glm::vec3 &up) { _upVec = up; }
  void setRightVec(const glm::vec3 &right) { _rightVec = right; }

private:
  glm::vec3 _upVec{0.0f, 1.0f, 0.0f}, _rightVec{1.0f, 0.0f, 0.0f};
  Type _type{Type::Invalid};

  bool _isMain{true};

  float _top, _bottom, _left, _right, _near, _far, _fovy, _aspect;
};