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
  if (_windowSystem) {
    delete _windowSystem;
    _windowSystem = nullptr;
  }
}

bool Engine::init(EngineInfo info) {
  bool succ = true;

  succ &= _windowSystem->init(info.windowInfo);

  return succ;
}

void Engine::start() {
  _state = State::RUNNING;
  while (!_windowSystem->shouldClose()) {
    _windowSystem->pollEvents();
    if (_state == State::RUNNING) {
    }
    if (_state == State::STOP) {
      break;
    }
  }
}

void Engine::pause() { _state = State::PAUSE; }

void Engine::resume() { _state = State::RUNNING; }

void Engine::stop() { _state = State::STOP; }