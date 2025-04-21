#pragma once

#include "common/common.h"
#include <functional>
#include <vector>

struct WindowInfo {
  int width, height;
  const char *title;
};

class WindowSystem {
public:
  using ResizeCallback = std::function<void(int, int)>;
  using KeyBoardCallback = std::function<void(int, int, int)>;
  using MouseCallback = std::function<void(int, int, int)>;
  using CursorCallback = std::function<void(double, double)>;

  WindowSystem() = default;
  ~WindowSystem();

  bool init(WindowInfo info);
  bool shouldClose();
  void pollEvents();
  void swapBuffers();

  void addResizeCallback(ResizeCallback callback) { _resizeCallbacks.push_back(callback); };
  void addKeyBoardCallback(KeyBoardCallback callback) { _keyBoardCallbacks.push_back(callback); }
  void addMouseCallback(MouseCallback callback) { _mouseCallbacks.push_back(callback); }
  void addCursorCallback(CursorCallback callback) { _cursorCallbacks.push_back(callback); }

  int getWidth() const { return _width; }
  int getHeight() const { return _height; }
  float getAspect() const { return (float)_width / _height; }
  void getCursorPosition(double &x, double &y) { glfwGetCursorPos(_window, &x, &y); }

private:
  int _width, _height;
  GLFWwindow *_window;

  std::vector<ResizeCallback> _resizeCallbacks{};
  std::vector<KeyBoardCallback> _keyBoardCallbacks{};
  std::vector<MouseCallback> _mouseCallbacks{};
  std::vector<CursorCallback> _cursorCallbacks{};

  static void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app)
      for (auto resizeCallback: app->_resizeCallbacks)
        resizeCallback(width, height);
  }

  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app)
      for (auto keyBoardCallback: app->_keyBoardCallbacks)
        keyBoardCallback(key, action, mods);
  }

  static void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app)
      for (auto mouseCallback: app->_mouseCallbacks)
        mouseCallback(button, action, mods);
  }

  static void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
    WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
    if (app)
      for (auto cursorCallback: app->_cursorCallbacks)
        cursorCallback(xpos, ypos);
  }
};