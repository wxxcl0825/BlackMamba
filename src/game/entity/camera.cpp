#include "game/entity/camera.h"
#include "game/game.h"
#include "game/utils/utils.h"
#include "runtime/framework/component/transform/transform.h"

Camera::Camera(std::shared_ptr<CameraComponent> cameraComp, Type type)
    : _type(type) {
  _camera = new GameObject();
  _transformComp = _camera->getComponent<TransformComponent>();
  _cameraComp = cameraComp;
  _camera->addComponent(_cameraComp);

  Game::getGame()->bind(
      Game::KeyBoard{},
      [this](int key, int action, int mods) { onKey(key, action, mods); });
  Game::getGame()->bind(Game::Mouse{}, [this](int button, int action,
                                              int mods) {
    double xpos = 0, ypos = 0;
    Game::getGame()->getEngine()->getWindowSystem()->getCursorPosition(xpos,
                                                                       ypos);
    onMouse(button, action, xpos, ypos);
  });
  Game::getGame()->bind(Game::Cursor{}, [this](double xpos, double ypos) {
    onCursor(xpos, ypos);
    _currentX = xpos, _currentY = ypos;
  });
  _camera->setTick([this]() { tick(); });
}

Camera::~Camera() {
  if (_camera) {
    delete _camera;
    _camera = nullptr;
  }
}

void Camera::enable() {
  if (_camera)
    _camera->getComponent<CameraComponent>()->pick();
}

void Camera::disable() {
  if (_camera)
    _camera->getComponent<CameraComponent>()->put();
}

void Camera::onKey(int key, int action, int mods) {
  _keyMap[key] = action == GLFW_PRESS  || action == GLFW_REPEAT;
}

void Camera::onCursor(double xpos, double ypos) {
  if (_type == Type::Free || _type == Type::FirstPersion) {
    float dx = (xpos - _currentX) * _sensitivity,
          dy = (ypos - _currentY) * _sensitivity;
    pitch(-dy);
    yaw(-dx);
  }
}

void Camera::onMouse(int button, int action, float xpos, float ypos) {
  bool pressed = action == GLFW_PRESS;
  if (pressed)
    _currentX = xpos, _currentY = ypos;
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    _leftMouseDown = pressed;
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    _rightMouseDown = pressed;
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    _middleMouseDown = pressed;
    break;
  }
}

void Camera::updatePosition() {
  if (_type == Type::Free) {
    glm::vec3 direction{0.0f};
    glm::vec3 front =
        glm::cross(_cameraComp->getUpVec(), _cameraComp->getRightVec());
    glm::vec3 right = _cameraComp->getRightVec();
    if (_keyMap[GLFW_KEY_UP])
      direction += front;
    if (_keyMap[GLFW_KEY_DOWN])
      direction -= front;
    if (_keyMap[GLFW_KEY_LEFT])
      direction -= right;
    if (_keyMap[GLFW_KEY_RIGHT])
      direction += right;
    if (glm::length(direction)) {
      direction = glm::normalize(direction);
      _transformComp->setPositionLocal(_transformComp->getPositionLocal() +
                                  direction * _speed);
    }
  }
}

void Camera::tick() {
  updatePosition();
  if (_type == Type::FirstPersion) {
    if (_camera && _camera->getParent()) {
      glm::mat4 model = _camera->getParent()->getComponent<TransformComponent>()->getModel();
      glm::vec3 pos, euler, scl;
      Utils::decompose(model, pos, euler, scl);
      _camera->getComponent<TransformComponent>()->setAngle(euler);
    }
  }
}

void Camera::pitch(float angle) {
  if (_type == Type::Free) {
    if (_pitch + angle > 89.0f || _pitch + angle < -89.0f)
      return;
    _pitch += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    _cameraComp->getRightVec());
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
  }
  if(_type == Type::FirstPersion) {
    glm::vec3 position, eulerAngle, scale;
    Utils::decompose(_camera->getComponent<TransformComponent>()->getParentModel(), position, eulerAngle, scale); 
    if (_pitch + angle > fmin(70.0f + eulerAngle.x, 89.0f) || _pitch + angle < fmax(-30.0f + eulerAngle.x, -89.0f))
      return;
    _pitch += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    _cameraComp->getRightVec());
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
  }
}

void Camera::yaw(float angle) {
  if (_type == Type::Free) {
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
    _cameraComp->setRightVec(glm::vec4(_cameraComp->getRightVec(), 0.0f) *
                             rotation);
  }
  if (_type == Type::FirstPersion) {
    if(_yaw + angle > 70.0f || _yaw + angle < -70.0f)
      return;
    _yaw += angle;
    glm::mat4 rotation =
        glm::rotate(glm::mat4(glm::one<float>()), glm::radians(angle),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    _cameraComp->setUpVec(glm::vec4(_cameraComp->getUpVec(), 0.0f) * rotation);
    _cameraComp->setRightVec(glm::vec4(_cameraComp->getRightVec(), 0.0f) *
                             rotation);
  }
}