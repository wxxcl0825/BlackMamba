#pragma once

#include "runtime/framework/object/gameObject.h"

#include <vector>

class AudioSystem {
public:
  AudioSystem() = default;
  ~AudioSystem() = default;
 
  
  bool init();
  void dispatch(GameObject *object);
  void tick();

private:
  std::vector<GameObject *> _audios;
  GameObject* _listener;

  void clear();
};