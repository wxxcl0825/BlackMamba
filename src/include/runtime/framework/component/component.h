#pragma once

#include "common/common.h"

class Component {
public:
  virtual ~Component() = 0;

private:
};

inline Component::~Component() {}