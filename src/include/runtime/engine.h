#pragma once

#include "framework/system/physicalSystem.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/system/renderSystem.h"
#include "runtime/framework/system/windowSystem.h"
#include "runtime/framework/system/physicalSystem.h"
#include "runtime/resource/resourceManager.h"

struct EngineInfo {
  WindowInfo windowInfo;
  PhysicsInfo physicsInfo;
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
  ResourceManager *getResourceManager() const { return _resourceManger; }
  PhysicalSystem *getPhysicalSystem() const { return _physicalSystem; }

  void setScene(GameObject *scene) { _scene = scene; }

private:
  static Engine *_engine;

  State _state{State::IDLE};

  WindowSystem *_windowSystem;
  RenderSystem *_renderSystem;
  ResourceManager *_resourceManger;
  PhysicalSystem *_physicalSystem;

  GameObject *_scene;

  Engine();
  ~Engine();

  void dispatch(GameObject* root);
  void tick();
};