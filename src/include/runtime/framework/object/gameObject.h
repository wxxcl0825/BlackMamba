#pragma once

#include "runtime/framework/component/component.h"
#include <functional>
#include <memory>
#include <vector>

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