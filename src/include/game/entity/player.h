#pragma once

#include <map>
#include <unordered_map>

#include "runtime/framework/object/gameObject.h"
#include "runtime/resource/material/material.h"

class Player {
public:
    Player(const std::string &path, Material *material, const glm::vec3 &position, const glm::vec3 &angle, const float &scale, const glm::vec3 &boxSize, const float &mass, const float &liftCoefficient, const float &maxAcceleration, const float &maxAngularAcceleration);
    ~Player();

    GameObject *getPlayer() const { return _player; }

    void setLiftCoefficient(const float &liftCoefficient) { _liftCoefficient = liftCoefficient; }
    void setMaxAcceleration(const float &maxAcceleration) { _maxAcceleration = maxAcceleration; }
    void setMaxAngularAcceleration(const float &maxAngularAcceleration) { _maxAngularAcceleration = maxAngularAcceleration; }

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

    void onKey(int key, int action, int mods);

    void tick();
};