#pragma once

#include "common/common.h"

struct WindowInfo {
  int width, height;
  const char *title;
};

class WindowSystem {
public:
  using ResizeCallback = void (*)(int width, int height);
  using KeyBoardCallback = void (*)(int key, int action, int mods);
  using MouseCallback = void (*)(int button, int action, int mods);
  using CursorCallback = void (*)(double xpos, double ypos);

  WindowSystem() = default;
  ~WindowSystem();

  bool init(WindowInfo info);
  bool shouldClose();
  void pollEvents();
  void swapBuffers();

  int getWidth() const { return _width; }
  int getHeight() const { return _height; }
  float getAspect() const { return (float)_width / _height; }

  void setResizeCallback(ResizeCallback callback) { _resizeCallback = callback; };
  void setKeyBoardCallback(KeyBoardCallback callback) { _keyBoardCallback = callback; }
  void setMouseCallback(MouseCallback callback) { _mouseCallback = callback; }
  void setCursorCallback(CursorCallback callback) { _cursorCallback = callback; }

private:
  int _width, _height;
  GLFWwindow *_window;

  ResizeCallback _resizeCallback{nullptr};
  KeyBoardCallback _keyBoardCallback{nullptr};
  MouseCallback _mouseCallback{nullptr};
  CursorCallback _cursorCallback{nullptr};

  static void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app && app->_resizeCallback)
      app->_resizeCallback(width, height);
  }

  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app && app->_keyBoardCallback)
      app->_keyBoardCallback(key, action, mods);
  }

  static void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app && app->_mouseCallback)
      app->_mouseCallback(button, action, mods);
  }

  static void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app && app->_cursorCallback)
      app->_cursorCallback(xpos, ypos);
  }
};