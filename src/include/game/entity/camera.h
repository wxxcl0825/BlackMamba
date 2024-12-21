#pragma once

#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"
#include <map>

class Camera {
public:
  enum class Type { Free, Invalid };

  Camera(CameraComponent *cameraComp, Type type);
  ~Camera();

  GameObject *getCamera() const { return _camera; }

private:
  GameObject *_camera{nullptr};

  TransformComponent *_transformComp{nullptr};
  CameraComponent *_cameraComp{nullptr};

  Type _type{Type::Invalid};

  std::map<int, bool> _keyMap;
  float _speed{1.0f};

  bool _leftMouseDown, _rightMouseDown, _middleMouseDown;
  float _currentX, _currentY;
  float _sensitivity{0.2f};

  float _pitch{0.0f};

  void onKey(int key, int action, int mods);
  void onMouse(int button, int action, float xpos, float ypos);
  void onCursor(double xpos, double ypos);

  void updatePosition();
  void pitch(float angle);
  void yaw(float angle);
};