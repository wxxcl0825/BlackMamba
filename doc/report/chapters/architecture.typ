#import "@preview/codly:1.1.1": *
#import "@preview/codly-languages:0.1.3": *

#let H(
  title: none
) = {
  codly(
    languages: codly-languages,
    header: [*#title*],
    header-cell-args: (align: center)
    )
}

= 架构设计

本项目参考games104@games104 介绍的引擎架构@piccolo 进行开发，组成如下

#figure(image("../images/2.1.png", width: 80%))

游戏由底层的引擎运行时与上层的游戏实现层构成，其中
- 运行时层由窗体系统(Window System), 渲染系统(Render System), 物理系统(Physical System), 音效系统(Audio System)与资源管理系统(Resource Manager)构成
  - 资源管理系统负责统一创建和销毁游戏中的各类资产，如顶点数据(Geometry), 纹理数据(Texture), 着色器(Shader)与音频(Audio)
  - 认为材质 = 纹理 + 着色器，对其进行单独封装，向游戏实现层提供扩展接口
- 游戏中一切实体皆为游戏对象(Game Objects)，游戏对象通过携带组建(Component)与系统与其它游戏对象进行交互
  - 基础组件包括：变换(Transform), 网格体(Mesh), 相机(Camera), 刚体(RigidBody), 光照(Light)和地形(Terrain)
- 游戏层则对运行时层提供的基础功能进行更加上层的封装，如游戏实体构建、定制化材质设计、相机控制器实现、场景布置等

引擎声明如下

#H(title: "include/runtime/engine.h")

```cpp
class Engine {
public:
  using MainLoop = std::function<void()>;

  enum class State { IDLE, RUNNING, PAUSE, STOP };

  static Engine *getEngine();
  static void destroyEngine();

  bool init(EngineInfo info);
  void start();
  void stop();
  void pause();
  void resume();

  State getState() const { return _state; }
  WindowSystem *getWindowSystem() const { return _windowSystem; }
  PhysicalSystem *getPhysicalSystem() const { return _physicalSystem; }
  AudioSystem *getAudioSystem() const { return _audioSystem; }
  ResourceManager *getResourceManager() const { return _resourceManger; }

  void setScene(GameObject *scene) { _scene = scene; }
  void setMainLoop(const MainLoop &mainloop) { _mainloop = mainloop; }

private:
  static Engine *_engine;

  State _state{State::IDLE};

  WindowSystem *_windowSystem;
  RenderSystem *_renderSystem;
  PhysicalSystem *_physicalSystem;
  AudioSystem *_audioSystem;
  ResourceManager *_resourceManger;

  GameObject *_scene;
  MainLoop _mainloop;

  Engine();
  ~Engine();

  void dispatch(GameObject *root);
  void tick();
};
```
- 引擎采用单例模式设计，确保全局只有唯一的引擎实例，通过私有构造函数实现
- 其中Game Object的根结点以Scene(场景)的形式在引擎中进行维护

引擎采用面向Game Object的形式进行更新, 第一阶段先深度遍历Game Object树，将所有游戏对象分发到各个系统中；第二阶段依次更新各系统，每个系统根据分发时记录的游戏对象进行更新，如图所示

#figure(image("../images/2.2.png", width: 60%), caption: "引擎更新逻辑")

实现如下

#H(title: "runtime/engine.cpp")

#figure()[
```cpp
void Engine::dispatch(GameObject *root) {
  if (root) {
    root->tick();
    _renderSystem->dispatch(root);
    _physicalSystem->dispatch(root);
    _audioSystem->dispatch(root);
    glm::mat4 rootModel = root->getComponent<TransformComponent>()-> getModel();
    for (auto child : root->getChildren()) {
      child->getComponent<TransformComponent>()->setParentModel(rootModel);
      dispatch(child);
    }
  }
}

void Engine::tick(){
  float dt = 1.0f;
  _physicalSystem->tick(dt);
  _renderSystem->tick(); 
  _audioSystem->tick();
}
```
]
<l1>

- 在分发过程中同时进行Model Matrix的下发，以维护父子游戏对象的变换继承关系

#colbreak()

游戏层则在引擎层之上继续封装，声明如下

#H(title: "include/game/game.h")

```cpp
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
```

#colbreak()

== 游戏对象

游戏对象即为游戏内一切实体，以树的形式进行管理。每个游戏对象负责维护父子关系，其中子物体继承父物体的所有变换；同时，游戏对象又是组件的载体，需维护自身具有的所有组件，并对外提供获取的接口。

游戏对象声明如下

#H(title: "include/runtime/framework/object/gameObject.h")

```cpp
class GameObject {
public:
  using MainLoop = std::function<void()>;

  GameObject();
  ~GameObject();

  template <typename ComponentType>
  std::shared_ptr<ComponentType> getComponent() const {
    for (const auto &component : _components) {
      auto targetComponent =
          std::dynamic_pointer_cast<ComponentType>(component);
      if (targetComponent)
        return targetComponent;
    }
    return nullptr;
  }

  void addChild(GameObject *object);
  void removeChild(GameObject *object);

  void tick() {
    if (_tick)
      _tick();
  }

  void setTick(const MainLoop &tick) { _tick = tick; }

  void addComponent(std::shared_ptr<Component> component);
  void setComponent(std::shared_ptr<Component> component);

  GameObject *getParent() const { return _parent; }
  const std::vector<GameObject *> &getChildren() const { return _children; }

private:
  std::vector<std::shared_ptr<Component>> _components{};

  MainLoop _tick{};

  GameObject *_parent{nullptr};
  std::vector<GameObject *> _children{};
};
```
- 由于游戏对象具有复杂的层级关系，为了对其进行高效的内存管理，使用智能指针对组件进行维护，以兼容游戏对象共享组件的设计
- 使用C++多态特性实现组件的统一管理

== 窗体系统

作为引擎实现的起点，窗体系统提供基本的窗体与事件管理功能。声明如下

#H(title: "include/runtime/framework/system/windowSystem.h")

```cpp
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
```
- 回调函数注册逻辑：由于GLFW仅允许使用外部定义或静态成员函数进行回调注册，故在类中封装出静态回调函数，负责调用外部加入的回调函数；利用GLFW提供的`glfwSetWindowUserPointer`和`glfwGetWindowUserPointer`，在静态函数中获取当前唯一的类对象的指针

#colbreak()

窗体初始化实现如下

#H(title: "runtime/framework/system/windowSystem.cpp")

```cpp
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
```
- 为了兼容 Apple silicon, 采用OpenGL 4.1 core profile 版本
- 使用GLAD进行算子选择