#include "game/game.h"
#include "runtime/framework/system/physicalSystem.h"

GameInfo info = {.enginInfo{
    .windowInfo {WindowInfo {  .width = 1280, 
                              .height = 720, 
                              .title = "Project: Black Mamba"}},
    .physicsInfo{PhysicsInfo{ .max_body_count = 10240,
                              .body_mutex_count = 0,
                              .max_body_pairs = 65536,
                              .max_contact_constraints = 10240,
                              .max_job_count = 1024,
                              .max_barrier_count = 8,
                              .max_concurrent_job_count = 4,
                              .gravity = {0.f, 0.f, -9.8f},
                              .update_frequency = 60.f}}
    }};

Game *game = nullptr;

int main() {
  game = Game::getGame();
  game->init(info);
  game->start();
  game->exit();
  return 0;
}