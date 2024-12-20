#include <string>
#include <vector>

#include "common/macro.h"
#include "include/runtime/resource/texture/texture.h"
#include "runtime/engine.h"
#include "runtime/framework/component/camera/camera.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/component/mesh/mesh.h"
#include "runtime/resource/resourceManager.h"
#include "runtime/resource/shader/shader.h"

EngineInfo info = {.windowInfo{
    WindowInfo{.width = 1280, .height = 720, .title = "Project: Black Mamba"}}};

Engine *engine = nullptr;

Camera *cameraComp = nullptr;

GameObject *scene = new GameObject();
GameObject *camera = new GameObject();

ResourceManager *resourceManager = nullptr;
Shader *shader = nullptr;
Texture *texture = nullptr;
Texture *skybox = nullptr;
std::vector<std::string> skyboxPaths = {
    "assets/textures/skybox/right.jpg",
    "assets/textures/skybox/left.jpg",
    "assets/textures/skybox/top.jpg",
    "assets/textures/skybox/bottom.jpg",
    "assets/textures/skybox/back.jpg",
    "assets/textures/skybox/front.jpg"};

void onResize(int width, int height) { glViewport(0, 0, width, height); }

void onKey(int key, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE)
    engine->stop();
  if (key == GLFW_KEY_SPACE) {
    if (engine->getState() == Engine::State::RUNNING)
      engine->pause();
    else
      engine->resume();
  }
}

void onMouse(int button, int action, int mods) {}

void onCursor(double xpos, double ypos) {}

int main() {
  engine = Engine::getEngine();
  if (!engine->init(info)) {
    Err("Engine init failed!");
  }

  engine->getWindowSystem()->setResizeCallback(onResize);
  engine->getWindowSystem()->setKeyBoardCallback(onKey);
  engine->getWindowSystem()->setMouseCallback(onMouse);
  engine->getWindowSystem()->setCursorCallback(onCursor);

  resourceManager = ResourceManager::getResourceManager();
  shader = resourceManager->loadShader("assets/shaders/default.vert", "assets/shaders/default.frag");
  texture = resourceManager->loadTexture("assets/textures/box.png");
  skybox = resourceManager->loadTexture(skyboxPaths);

  resourceManager = ResourceManager::getResourceManager();
  shader = resourceManager->loadShader("assets/shaders/default.vert", "assets/shaders/default.frag");
  texture = resourceManager->loadTexture("assets/textures/box.png");
  skybox = resourceManager->loadTexture(skyboxPaths);

  cameraComp = new Camera(45.0f, engine->getWindowSystem()->getAspect(), 0.1f, 1000.0f);
  camera->addComponent(cameraComp);

  scene->addChild(camera);
  scene->addChild(new GameObject());
  scene->addChild(new GameObject());
  scene->addComponent(new Mesh(resourceManager->createBoxGeometry(1.0f), new Material()));

  Mesh* mesh = scene->getComponent<Mesh>();
  Log("mesh == nullptr?: %d", mesh == nullptr);

  engine->setScene(scene);

  engine->start();
  delete scene;
  Engine::destroyEngine();
  return 0;
}