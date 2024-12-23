#include "game/game.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "common/macro.h"
#include "game/entity/camera.h"
#include "game/entity/player.h"
#include "game/entity/skybox.h"
#include "game/entity/terrain.h"
#include "game/material/phongMaterial.h"
#include "game/material/terrainMaterial.h"
#include "runtime/framework/component/audio/audio.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/component/light/light.h"
#include "runtime/framework/component/physics/rigidBody.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/system/jolt/utils.h"
#include <memory>

Game *Game::_game = nullptr;

Game *Game::getGame() {
  if (!_game) {
    _game = new Game();
  }
  return _game;
}

Game::~Game() {
  if (_scene) {
    delete _scene;
    _scene = nullptr;
  }
  Engine::destroyEngine();
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
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
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
          45.0f, _engine->getWindowSystem()->getAspect(), 0.1f, 10000.0f),
      Camera::Type::Free);

  camera->setSpeed(1.0f);

  _scene->addComponent(
      std::make_shared<LightComponent>(glm::vec3(1.0f, 1.0f, 1.0f)));

  Skybox *skybox = new Skybox(
      {"assets/textures/skybox/right.jpg", "assets/textures/skybox/left.jpg",
       "assets/textures/skybox/top.jpg", "assets/textures/skybox/bottom.jpg",
       "assets/textures/skybox/back.jpg", "assets/textures/skybox/front.jpg"});

  skybox->bind(camera->getCamera());

  _scene->addChild(camera->getCamera());

  PhongMaterial *phongMat = new PhongMaterial();
  Player *player =
      new Player("assets/models/fighter/fighter.obj", phongMat,
                 glm::vec3(0.0f, 175.0f, -415.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                 3.0f, glm::vec3(50.0f), 1.0f, 0.7f, 18.0f, 0.02f);

  std::shared_ptr<AudioComponent> audioComp =
      std::make_shared<AudioComponent>();
  audioComp->append(
      _engine->getResourceManager()->loadAudio("assets/audios/engine.ogg"));
  audioComp->start();
  audioComp->setMode(AudioComponent::Mode::REPEAT_SINGEL);
  player->getPlayer()->addComponent(audioComp);

  Camera *fCamera = new Camera(
      std::make_shared<CameraComponent>(
          60.0f, _engine->getWindowSystem()->getAspect(), 0.1f, 10000.0f),
      Camera::Type::FirstPersion);
  fCamera->disable();
  fCamera->getCamera()->getComponent<TransformComponent>()->setPositionLocal(
      glm::vec3(0.0f, 1.75f, 4.15f));
  fCamera->getCamera()->getComponent<CameraComponent>()->setRightVec(
      glm::vec3(-1.0f, 0.0f, 0.0f));
  fCamera->getCamera()->getComponent<CameraComponent>()->setUpVec(
      glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f)));

  player->getPlayer()->addChild(fCamera->getCamera());

  Game::getGame()->bind(KeyBoard{}, [player, camera, fCamera,
                                     skybox](int key, int action, int mods) {
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
      camera->disable();
      fCamera->enable();
      skybox->bind(fCamera->getCamera());
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
      fCamera->disable();
      camera->enable();
      skybox->bind(camera->getCamera());
      camera->getCamera()->getComponent<TransformComponent>()->setPositionLocal(
          glm::inverse(camera->getCamera()
                           ->getComponent<TransformComponent>()
                           ->getParentModel()) *
          glm::vec4((player->getPlayer()
                         ->getComponent<TransformComponent>()
                         ->getPositionWorld() +
                     glm::vec3(0.0f, 1.0f, -3.0f)),
                    1.0f));
    }
  });

  TerrainMaterial *terrainMat = new TerrainMaterial();
  terrainMat->setDiffuse(_engine->getResourceManager()->loadTexture(
      "assets/textures/terrain/diffuse.jpg"));
  terrainMat->setHeightMap(_engine->getResourceManager()->loadTexture(
      "assets/textures/terrain/heightMap.png"));
  Terrain *terrain = new Terrain(10000.0f, 10000.0f, 200, 100, terrainMat);
  terrain->getTerrain()->addComponent(std::make_shared<RigidBodyComponent>(
      JPH::EMotionType::Static, Layers::STATIC, 100000.0f, 1.f, 100000.0f,
      1.0f));

  _scene->addChild(player->getPlayer());
  _scene->addChild(terrain->getTerrain());

  _engine->setMainLoop([] {});
}