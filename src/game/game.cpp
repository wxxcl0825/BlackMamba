#include "game/game.h"
#include "common/macro.h"
#include "game/entity/camera.h"
#include "game/entity/skybox.h"
#include "game/material/phongMaterial.h"
#include "game/utils/utils.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/object/gameObject.h"
#include <memory>

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
      std::make_shared<CameraComponent>(
          45.0f, _engine->getWindowSystem()->getAspect(), 0.1f, 1000.0f),
      Camera::Type::Free);

  _scene->addComponent(
      std::make_shared<LightComponent>(glm::vec3(1.0f, 1.0f, 1.0f)));

  Skybox *skybox = new Skybox(
      {"assets/textures/skybox/right.jpg", "assets/textures/skybox/left.jpg",
       "assets/textures/skybox/top.jpg", "assets/textures/skybox/bottom.jpg",
       "assets/textures/skybox/back.jpg", "assets/textures/skybox/front.jpg"});

  skybox->bind(camera->getCamera());

  _scene->addChild(camera->getCamera());

  PhongMaterial *material = new PhongMaterial();
  GameObject *model = loadModel("assets/models/bag/backpack.obj", *material);
  _scene->addChild(model);
  _scene->addChild(skybox->getSkybox());
}