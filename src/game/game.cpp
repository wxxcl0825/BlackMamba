#include "game/game.h"
#include "common/macro.h"
#include "game/entity/camera.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/resource/material/whiteMaterial.h"
#include "runtime/resource/resourceManager.h"

Game *Game::_game = nullptr;

Game *Game::getGame() {
  if (!_game) {
    _game = new Game();
  }
  return _game;
}

Game::~Game() {
  Engine::destroyEngine();
  if (_scene) {
    delete _scene;
    _scene = nullptr;
  }
}

void Game::init(GameInfo info) {
  if (!_engine->init(info.enginInfo)) {
    Err("Engine init failed!");
  }
  bind(Resize{},
       [](int width, int height) { glViewport(0, 0, width, height); });
  bind(KeyBoard{}, [](int key, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE)
      Engine::getEngine()->stop();
    if (key == GLFW_KEY_SPACE) {
      if (Engine::getEngine()->getState() == Engine::State::RUNNING)
        Engine::getEngine()->pause();
      else
        Engine::getEngine()->resume();
    }
  });
  setupScene();
  _engine->setScene(_scene);
}

void Game::start() { _engine->start(); }

void Game::exit() {
  if (_game) {
    delete _game;
    _game = nullptr;
  }
}

void Game::setupScene() {
  _scene = new GameObject();

  Camera *camera = new Camera(
      new CameraComponent(45.0f, _engine->getWindowSystem()->getAspect(), 0.1f,
                          1000.0f),
      Camera::Type::Free);

  _scene->addComponent(new MeshComponent(
      ResourceManager::getResourceManager()->createBoxGeometry(1.0f),
      new WhiteMaterial()));

  _scene->addChild(camera->getCamera());
}