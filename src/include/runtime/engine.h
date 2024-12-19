#pragma once

#include "common/common.h"
#include "runtime/framework/system/windowSystem.h"

struct EngineInfo {
  WindowInfo windowInfo;
};

class Engine {
public:
  static Engine *getEngine();
  static void destroyEngine();

  bool init(EngineInfo info);
  void start();
  void resume();
  void stop();

  WindowSystem *getWindowSystem() const { return _windowSystem; };

private:
  static Engine *_engine;

  bool _pause{false};

  WindowSystem *_windowSystem;

  Engine();
  ~Engine();
};