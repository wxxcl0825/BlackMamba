#include "runtime/framework/system/windowSystem.h"
#include "common/macro.h"

WindowSystem::~WindowSystem() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

bool WindowSystem::init(WindowInfo info) {
  _width = info.width, _height = info.height;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(_width, _height, info.title, NULL, NULL);
  if (_window == nullptr) {
    Log("Fail to create window!");
    return false;
  }
  glfwMakeContextCurrent(_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Log("Fail to initialize GLAD!");
    return false;
  }

  glfwSetFramebufferSizeCallback(_window, frameBufferSizeCallback);
  glfwSetKeyCallback(_window, keyCallback);
  glfwSetMouseButtonCallback(_window, mouseCallback);
  glfwSetCursorPosCallback(_window, cursorCallback);
  glfwSetWindowUserPointer(_window, this);

  return true;
}

bool WindowSystem::shouldClose() { return glfwWindowShouldClose(_window); }

void WindowSystem::pollEvents() { glfwPollEvents(); }

void WindowSystem::swapBuffers() { glfwSwapBuffers(_window); }