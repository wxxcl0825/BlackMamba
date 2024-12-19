#include "runtime/engine.h"

Engine *Engine::_engine = nullptr;

Engine *Engine::getEngine() {
  if (!_engine) {
    _engine = new Engine();
  }
  return _engine;
}

void Engine::destroyEngine() {
  if (_engine) {
    delete _engine;
    _engine = nullptr;
  }
}

Engine::Engine() { _windowSystem = new WindowSystem(); }

Engine::~Engine() {
  if (_windowSystem)
    delete _windowSystem;
}

bool Engine::init(EngineInfo info) {
  bool succ = true;

  succ &= _windowSystem->init(info.windowInfo);

  return succ;
}

void Engine::start() {
  while (!_windowSystem->shouldClose()) {
    _windowSystem->pollEvents();
    if (!_pause) {
    }
  }
}

void Engine::resume() { _pause = false; }

void Engine::stop() { _pause = true; }