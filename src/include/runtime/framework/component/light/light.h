#pragma once

#include "common/common.h"
#include "runtime/framework/component/component.h"

struct DirectionalLight {
  glm::vec3 color;
  glm::vec3 direction;
  float specularIntensity;
};

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  float specularIntensity;
  float k2;
  float k1;
  float kc;
};

struct SpotLight {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 direction;
  float specularIntensity;
  float inner;
  float outer;
};

struct AmbientLight {
  glm::vec3 color;
};

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

  Type getType() const { return _type; }
  glm::vec3 getColor() const { return _color; }
  glm::vec3 getDirection() const { return _direction; }
  float getSpecularIntensity() const { return _specularIntensity; }
  float getK2() const { return _k2; }
  float getK1() const { return _k1; }
  float getKc() const { return _kc; }
  float getInner() const { return _inner; }
  float getOuter() const { return _outer; }

private:
  glm::vec3 _color, _direction;
  float _specularIntensity, _k2, _k1, _kc, _inner, _outer;

  Type _type{Type::Invalid};
};