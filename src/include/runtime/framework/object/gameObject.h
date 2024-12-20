#pragma once

#include "common/common.h"

#include "runtime/framework/component/component.h"
#include <vector>

class GameObject {
public:
  GameObject();
  ~GameObject();

  template <typename ComponentType> ComponentType *getComponent() const {
    for (Component *component : _components) {
      ComponentType *targetComponent = dynamic_cast<ComponentType *>(component);
      if (targetComponent)
        return targetComponent;
    }
    return nullptr;
  }

  void addChild(GameObject *object);
  void removeChild(GameObject *object);
  void addComponent(Component *component);

  GameObject *getParent() const { return _parent; }
  const std::vector<GameObject *> &getChildren() const { return _children; }

private:
  std::vector<Component *> _components;

  GameObject *_parent;
  std::vector<GameObject *> _children;
};