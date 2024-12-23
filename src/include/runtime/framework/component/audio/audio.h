#pragma once

#include "runtime/framework/component/component.h"

class AudioComponent : public Component {
public:
  enum class Mode { SINGLE, REPEAT, Invalide };

private:
  Mode _mode{Mode::Invalide};
};