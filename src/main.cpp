#include "common/macro.h"
#include "runtime/engine.h"
#include "runtime/framework/object/gameObject.h"
#include "runtime/framework/component/mesh/mesh.h"

EngineInfo info = {.windowInfo{
    WindowInfo{.width = 1280, .height = 720, .title = "Project: Black Mamba"}}};

Engine *engine = nullptr;

GameObject *scene = new GameObject();

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

  scene->addChild(new GameObject());
  scene->addChild(new GameObject());
  scene->addComponent(new Mesh());

  Mesh* mesh = scene->getComponent<Mesh>();
  Log("mesh == nullptr?: %d", mesh == nullptr);

  engine->setScene(scene);

  engine->start();
  delete scene;
  Engine::destroyEngine();
  return 0;
}