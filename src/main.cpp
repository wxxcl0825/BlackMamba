#include "game/game.h"
#include "runtime/framework/system/physicalSystem.h"

GameInfo info = {.enginInfo{
    .windowInfo {WindowInfo {  .width = 1280, 
                              .height = 720, 
                              .title = "Project: Black Mamba"}},
    .physicsInfo{PhysicsInfo{ .maxBodyCount = 10240,
                              .bodyMutexCount = 0,
                              .maxBodyPairs = 65536,
                              .maxContactConstraints = 10240,
                              .maxJobCount = 1024,
                              .maxBarrierCount = 8,
                              .maxConcurrentJobCount = 4,
                              .gravity = {0.f, -9.8f, 0.0f},
                              .updateFrequency = 60.f,
                              .collisionSteps = 1}}
    }
};

Game *game = nullptr;

int main() {
  game = Game::getGame();
  game->init(info);
  game->start();
  game->exit();
  return 0;
}