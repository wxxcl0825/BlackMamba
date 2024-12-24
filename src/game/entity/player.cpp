#include "game/entity/player.h"

#include "game/game.h"
#include "game/material/explosionMaterial.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/framework/component/physics/rigidBody.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"

Player::Player(GameObject *player, const glm::vec3 &position,
               const glm::vec3 &angle, const float &scale,
               const glm::vec3 &boxSize, const float &mass,
               const float &liftCoefficient, const float &maxAcceleration,
               const float &maxAngularAcceleration)
    : _player(player), _position(position), _angle(angle), _scale(scale),
      _boxSize(boxSize), _mass(mass), _liftCoefficient(liftCoefficient),
      _maxAcceleration(maxAcceleration),
      _maxAngularAcceleration(maxAngularAcceleration) {
  _player->getComponent<TransformComponent>()->setPositionLocal(_position);
  _player->getComponent<TransformComponent>()->setAngle(_angle);
  _player->getComponent<TransformComponent>()->setScale(glm::vec3(_scale));
  _player->addComponent(std::make_shared<RigidBodyComponent>(
      JPH::EMotionType::Dynamic, Layers::MOVING, boxSize.x, boxSize.y,
      boxSize.z, mass / (boxSize.x * boxSize.y * boxSize.z)));
  Game::getGame()->bind(
      Game::KeyBoard{},
      [this](int key, int action, int mods) { onKey(key, action, mods); });
  _player->setTick([this]() { tick(); });
}

Player::~Player() {
  if (_player) {
    delete _player;
    _player = nullptr;
  }
}

void Player::onKey(int key, int action, int mods) {
  _keyMap[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
}

void Player::tick() {
  if (_onExplode) {
    if (glfwGetTime() >= _explodeEnd) {
      _player->getParent()->removeChild(_player);
      return;
    }
    _explode(_player, _explosionFunc(glfwGetTime() - _explodeStart));
    return;
  }

  if(_canExplode && _player->getComponent<RigidBodyComponent>()->getLinearVelocity().y < -3.0f && _player->getComponent<RigidBodyComponent>()->isCollide()){
    explode();
    _canExplode = false;
  }

  // already normalized
  glm::vec3 forward =
      _player->getComponent<TransformComponent>()->getForwardVec();
  glm::vec3 right = _player->getComponent<TransformComponent>()->getRightVec();
  glm::vec3 up = _player->getComponent<TransformComponent>()->getUpVec();

  // set force
  glm::vec3 engineForce = glm::vec3{0.0f};
  if (_keyMap[GLFW_KEY_SPACE]) {
    engineForce += forward * _maxAcceleration * _mass;
  }

  glm::vec3 liftForce = glm::vec3{0.0f};
  glm::vec3 linearVelocity =
      _player->getComponent<RigidBodyComponent>()->getLinearVelocity();
  liftForce = static_cast<float>(std::pow(_liftCoefficient, 2)) *
              glm::length(linearVelocity) * up;

  if(glm::length(liftForce) > glm::length(engineForce) * 0.5f)
    liftForce = glm::normalize(liftForce) * glm::length(engineForce) * 0.5f;

  _player->getComponent<RigidBodyComponent>()->setForce(engineForce +
                                                        liftForce);

  // set torque
  glm::vec3 torque = glm::vec3{0.0f};
  if (_keyMap[GLFW_KEY_W]) {
    torque +=
        right * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.y, 2) + std::pow(_boxSize.z, 2)) *
        _mass;
  }
  if (_keyMap[GLFW_KEY_S]) {
    torque +=
        -right * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.y, 2) + std::pow(_boxSize.z, 2)) *
        _mass;
  }
  if (_keyMap[GLFW_KEY_A]) {
    torque +=
        -forward * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.x, 2) + std::pow(_boxSize.y, 2)) *
        _mass;
  }
  if (_keyMap[GLFW_KEY_D]) {
    torque +=
        forward * _maxAngularAcceleration * (1.0f / 12.0f) *
        static_cast<float>(std::pow(_boxSize.x, 2) + std::pow(_boxSize.y, 2)) *
        _mass;
  }

  _player->getComponent<RigidBodyComponent>()->setTorque(torque);
}

void Player::_explode(GameObject *root, float time) {
  if (root->getComponent<MeshComponent>()) {
    Material *mat = root->getComponent<MeshComponent>()->getMaterial();
    if (dynamic_cast<ExplosionMaterial *>(mat)) {
      ExplosionMaterial *explosionMat = (ExplosionMaterial *)mat;
      explosionMat->setTime(time);
    }
  }
  for (auto child : root->getChildren())
    _explode(child, time);
}