#pragma once

#include "runtime/framework/system/windowSystem.h"

struct EngineInfo {
  WindowInfo windowInfo;
};

class Engine {
public:
  enum class State { IDLE, RUNNING, PAUSE, STOP };

  static Engine *getEngine();
  static void destroyEngine();

  bool init(EngineInfo info);
  void start();
  void stop();
  void pause();
  void resume();

  State getState() const { return _state; }
  WindowSystem *getWindowSystem() const { return _windowSystem; }

private:
  static Engine *_engine;

  State _state{State::IDLE};

  WindowSystem *_windowSystem;

  Engine();
  ~Engine();
};