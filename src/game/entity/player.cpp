#include "game/entity/player.h"

#include "game/game.h"
#include "game/utils/utils.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/component/physics/rigidBody.h"

Player::Player(const std::string &path, Material *material, const glm::vec3 &position, const glm::vec3 &angle, const float &scale, const glm::vec3 &boxSize, const float &mass, const float &liftCoefficient, const float &maxAcceleration, const float &maxAngularAcceleration) 
: _position(position), _angle(angle), _scale(scale), _boxSize(boxSize), _mass(mass), _liftCoefficient(liftCoefficient), _maxAcceleration(maxAcceleration), _maxAngularAcceleration(maxAngularAcceleration) {
    _player = Utils::loadModel(path, *material);
    _player->getComponent<TransformComponent>()->setPositionLocal(_position);
    _player->getComponent<TransformComponent>()->setAngle(_angle);
    _player->getComponent<TransformComponent>()->setScale(glm::vec3(_scale));
    _player->addComponent(std::make_shared<RigidBodyComponent>(JPH::EMotionType::Dynamic, Layers::MOVING, boxSize.x, boxSize.y, boxSize.z, mass/(boxSize.x*boxSize.y*boxSize.z)));
    Game::getGame()->bind(
        Game::KeyBoard{}, 
        [this](int key, int action, int mods){onKey(key, action, mods);});
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

void Player::tick(){
    glm::vec3 forward = _player->getComponent<TransformComponent>()->getForwardVec();
    glm::vec3 right = _player->getComponent<TransformComponent>()->getRightVec();
    glm::vec3 up = _player->getComponent<TransformComponent>()->getUpVec();

    // set force
    glm::vec3 engineForce = glm::vec3{0.0f};
    if(_keyMap[GLFW_KEY_SPACE]){
        engineForce += glm::normalize(forward) * _maxAcceleration * _mass;
    }

    glm::vec3 liftForce = glm::vec3{0.0f};
    glm::vec3 linearVelocity = _player->getComponent<RigidBodyComponent>()->getLinearVelocity();
    liftForce = glm::length(linearVelocity) * glm::normalize(up) * _liftCoefficient;

    _player->getComponent<RigidBodyComponent>()->setForce(engineForce+liftForce);

    // set torque
    glm::vec3 torque = glm::vec3{0.0f};
    if(_keyMap[GLFW_KEY_W]){
        torque += glm::normalize(right) * _maxAngularAcceleration * _mass;
    }
    if(_keyMap[GLFW_KEY_S]){
        torque += -glm::normalize(right) * _maxAngularAcceleration * _mass;
    }
    if(_keyMap[GLFW_KEY_A]){
        torque += -glm::normalize(up) * _maxAngularAcceleration * _mass;
    }
    if(_keyMap[GLFW_KEY_D]){
        torque += glm::normalize(up) * _maxAngularAcceleration * _mass;
    }

    _player->getComponent<RigidBodyComponent>()->setTorque(torque);
}