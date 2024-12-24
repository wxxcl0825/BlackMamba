#pragma once

#include <functional>
#include <map>
#include <unordered_map>

#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/material/material.h"

class Player {
public:
  using ExplosionFunc = std::function<float(float)>;

  Player(GameObject *player, const glm::vec3 &position, const glm::vec3 &angle,
         const float &scale, const glm::vec3 &boxSize, const float &mass,
         const float &liftCoefficient, const float &maxAcceleration,
         const float &maxAngularAcceleration);
  ~Player();

  GameObject *getPlayer() const { return _player; }

  void explode() {
    if (!_onExplode) {
      _onExplode = true, _explodeStart = glfwGetTime(),
      _explodeEnd = _explodeStart + _explodeDelta;
    }
  }

  void setLiftCoefficient(const float &liftCoefficient) {
    _liftCoefficient = liftCoefficient;
  }
  void setMaxAcceleration(const float &maxAcceleration) {
    _maxAcceleration = maxAcceleration;
  }
  void setMaxAngularAcceleration(const float &maxAngularAcceleration) {
    _maxAngularAcceleration = maxAngularAcceleration;
  }

  void setExplosionFunc(const ExplosionFunc &func) { _explosionFunc = func; }

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

  std::unordered_map<int, bool> _keyMap;

  float _explodeDelta{2.0f};

  bool _onExplode{false};
  float _explodeStart{.0f};
  float _explodeEnd{.0f};

  ExplosionFunc _explosionFunc{[](float x) -> float { return x; }};

  void _explode(GameObject *root, float time);

  void onKey(int key, int action, int mods);

  void tick();
};