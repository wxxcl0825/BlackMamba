#pragma once

#include "common/common.h"

struct WindowInfo {
  int width, height;
  const char *title;
};

class WindowSystem {
public:
  WindowSystem() = default;
  ~WindowSystem();

  bool init(WindowInfo info);
  bool shouldClose();
  void pollEvents();

private:
  int _width, _height;
  GLFWwindow *_window;
};