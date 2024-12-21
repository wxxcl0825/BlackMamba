#pragma once

#include "runtime/engine.h"

struct GameInfo {
  EngineInfo enginInfo;
};

class Game {
public:
  struct Resize {};
  struct KeyBoard {};
  struct Mouse {};
  struct Cursor {};

  static Game *getGame();

  void init(GameInfo info);
  void start();
  void exit();

  void bind(Resize, WindowSystem::ResizeCallback callback) {
    _engine->getWindowSystem()->addResizeCallback(callback);
  }
  void bind(KeyBoard, WindowSystem::KeyBoardCallback callback) {
    _engine->getWindowSystem()->addKeyBoardCallback(callback);
  }
  void bind(Mouse, WindowSystem::MouseCallback callback) {
    _engine->getWindowSystem()->addMouseCallback(callback);
  }
  void bind(Cursor, WindowSystem::CursorCallback callback) {
    _engine->getWindowSystem()->addCursorCallback(callback);
  }

  Engine *getEngine() { return _engine; }
  GameObject *getScene() { return _scene; }

private:
  static Game *_game;

  Engine *_engine{Engine::getEngine()};

  GameObject *_scene{nullptr};

  Game() = default;
  ~Game();

  void setupScene();
};