#include "common/macro.h"
#include "runtime/engine.h"

int main() {
  EngineInfo info = {.windowInfo{WindowInfo{
      .width = 1280, .height = 720, .title = "Project: Black Mamba"}}};

  Engine *engine = Engine::getEngine();
  if (!engine->init(info)) {
    Err("Engine init failed!");
  }
  engine->start();
  Engine::destroyEngine();
  return 0;
}