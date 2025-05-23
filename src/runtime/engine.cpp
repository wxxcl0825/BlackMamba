#include "runtime/engine.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/system/audioSystem.h"

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

Engine::Engine() {
  _windowSystem = new WindowSystem();
  _renderSystem = new RenderSystem();
  _resourceManger = new ResourceManager();
  _physicalSystem = new PhysicalSystem();
  _audioSystem = new AudioSystem();
}

Engine::~Engine() {
  if (_windowSystem) {
    delete _windowSystem;
    _windowSystem = nullptr;
  }
  if (_renderSystem) {
    delete _renderSystem;
    _renderSystem = nullptr;
  }
  if (_resourceManger) {
    delete _resourceManger;
    _resourceManger = nullptr;
  }
  if(_physicalSystem){
    delete _physicalSystem;
    _physicalSystem = nullptr;
  }
  if (_audioSystem) {
    delete _audioSystem;
    _audioSystem = nullptr;
  }
}

bool Engine::init(EngineInfo info) {
  bool succ = true;

  succ &= _windowSystem->init(info.windowInfo);
  succ &= _physicalSystem->init(info.physicsInfo);
  succ &= _audioSystem->init();

  return succ;
}

void Engine::start() {
  _state = State::RUNNING;
  while (!_windowSystem->shouldClose()) {
    _windowSystem->pollEvents();
    if (_state == State::RUNNING) {
      _mainloop();
      dispatch(_scene);
      tick();
      _windowSystem->swapBuffers();
    }
    if (_state == State::STOP) {
      break;
    }
  }
}

void Engine::pause() { 
  _resourceManger->pauseAllAudios();
  _state = State::PAUSE; 
}

void Engine::resume() { 
  _resourceManger->startAllAudios();
  _state = State::RUNNING; 
}

void Engine::stop() { _state = State::STOP; }

void Engine::dispatch(GameObject *root) {
  if (root) {
    root->tick();
    _renderSystem->dispatch(root);
    _physicalSystem->dispatch(root);
    _audioSystem->dispatch(root);
    glm::mat4 rootModel = root->getComponent<TransformComponent>()->getModel();
    for (auto child : root->getChildren()) {
      child->getComponent<TransformComponent>()->setParentModel(rootModel);
      dispatch(child);
    }
  }
}

void Engine::tick(){
  float dt = 1.0f;
  _physicalSystem->tick(dt);
  _renderSystem->tick(); 
  _audioSystem->tick();
}