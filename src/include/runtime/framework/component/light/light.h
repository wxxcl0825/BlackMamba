#pragma once

#include "common/common.h"
#include "runtime/framework/component/component.h"

class Light : public Component {
public:
  enum class Type { Directional, Point, Spot, Ambient, Invalid };

  Light(const glm::vec3 &color) : _color(color), _type(Type::Ambient) {}
  Light(const glm::vec3 &color, const glm::vec3 &direction,
        float specularIntensity)
      : _color(color), _direction(direction),
        _specularIntensity(specularIntensity), _type(Type::Directional) {}
  Light(const glm::vec3 &color, float specularIntensity, float k2, float k1,
        float kc)
      : _color(color), _specularIntensity(specularIntensity), _k2(k2), _k1(k1),
        _kc(kc), _type(Type::Point) {}
  Light(const glm::vec3 &color, const glm::vec3 &direction,
        float specularIntensity, float inner, float outer)
      : _color(color), _direction(direction),
        _specularIntensity(specularIntensity), _inner(inner), _outer(outer),
        _type(Type::Spot) {}
  ~Light() override{};

private:
  glm::vec3 _color, _direction;
  float _specularIntensity, _k2, _k1, _kc, _inner, _outer;

  Type _type{Type::Invalid};
};