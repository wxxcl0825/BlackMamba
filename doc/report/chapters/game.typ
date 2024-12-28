= 游戏实现
== 相机控制
== 模型导入
== 飞行控制
飞机在飞行时，需要考虑到重力、空气阻力、升力等因素。首先，飞机自身有一个引擎的动力，推动飞机向机头方向运动，其大小固定，对飞机产生固定大小的加速度；同时，飞机的机翼产生升力，该升力的方向朝向飞机模型的上方，其大小与飞机速度的平方成正比；此外，飞机的机身受到空气阻力的影响，空气阻力的方向与飞机的运动方向相反，大小也与速度成正比；最后，飞机可以通过控制升降舵和方向舵，给飞机自身施加转向力矩，控制飞机的滚转、俯仰和偏航。

玩家类（即飞机）的定义如下：
```cpp
class Player {
public:
  using ExplosionFunc = std::function<float(float)>;

  Player(GameObject *player, const glm::vec3 &position, const glm::vec3 &angle,
         const float &scale, const glm::vec3 &boxSize, const float &mass,
         const float &liftCoefficient, const float &maxAcceleration,
         const float &maxAngularAcceleration);
  ~Player();

  GameObject *getPlayer() const { return _player; }

  void setLiftCoefficient(const float &liftCoefficient) {
    _liftCoefficient = liftCoefficient;
  }
  void setMaxAcceleration(const float &maxAcceleration) {
    _maxAcceleration = maxAcceleration;
  }
  void setMaxAngularAcceleration(const float &maxAngularAcceleration) {
    _maxAngularAcceleration = maxAngularAcceleration;
  }

private:
  GameObject *_player{nullptr};

  glm::vec3 _position;
  glm::vec3 _angle;
  float _scale;

  glm::vec3 _boxSize;
  float _mass;
  float _liftCoefficient;
  float _maxAcceleration;
  float _maxAngularAcceleration;

  void tick();
};
```
其受力模型的实现如下：
```cpp
void Player::tick() {
  if (_onExplode) {
    if (glfwGetTime() >= _explodeEnd) {
      _player->getParent()->removeChild(_player);
      return;
    }
    _explode(_player, _explosionFunc(glfwGetTime() - _explodeStart));
    return;
  }

  if(_canExplode && _player->getComponent<RigidBodyComponent>()->getLinearVelocity().y < -0.6f && _player->getComponent<RigidBodyComponent>()->isCollide()){
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
```