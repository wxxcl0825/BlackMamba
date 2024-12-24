#pragma once

#include "game/material/phongMaterial.h"
#include "runtime/framework/system/renderSystem.h"
#include "runtime/resource/material/material.h"

class ExplosionMaterial : public PhongMaterial {
public:
  ExplosionMaterial() : PhongMaterial() { prepareShader(); }
  ExplosionMaterial(const ExplosionMaterial &other) : PhongMaterial(other) {
    _time = other._time;
  }
  ExplosionMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    prepareShader();
    _time = .0f;
  }
  ExplosionMaterial(const Material &other) : PhongMaterial(other) {
    prepareShader();
    _time = .0f;
  }
  ~ExplosionMaterial() override {}

  ExplosionMaterial *clone() const override {
    return new ExplosionMaterial(*this);
  }
  void apply(const RenderInfo &info) override;

  void setTime(float time) { _time = time; }

private:
  float _time{0.0f};

  void prepareShader();
};