#include "game/game.h"

GameInfo info = {.enginInfo{.windowInfo{WindowInfo{
    .width = 1280, .height = 720, .title = "Project: Black Mamba"}}}};

Game *game = nullptr;

int main() {
  game = Game::getGame();
  game->init(info);
  game->start();
  game->exit();
  return 0;
}