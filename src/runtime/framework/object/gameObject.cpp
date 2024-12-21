#include "runtime/framework/object/gameObject.h"

#include "common/macro.h"
#include "runtime/framework/component/transform/transform.h"
#include <algorithm>

GameObject::GameObject() {
  addComponent(std::make_shared<TransformComponent>());
}

GameObject::~GameObject() {
  _components.clear();
  for (auto child : _children)
    if (child) {
      delete child;
      child = nullptr;
    }
}

void GameObject::addChild(GameObject *object) {
  if (std::find(_children.begin(), _children.end(), object) !=
      _children.end()) {
    Log("Duplicated child added!");
    return;
  }
  object->_parent = this;
  _children.push_back(object);
}

void GameObject::removeChild(GameObject *object) {
  auto child = std::find(_children.begin(), _children.end(), object);
  if (child == _children.end()) {
    Log("Remove failed! Child not found!");
    return;
  }
  _children.erase(child);
  object->_parent = nullptr;
  delete object;
}

void GameObject::addComponent(std::shared_ptr<Component> component) {
  if (std::find(_components.begin(), _components.end(), component) !=
      _components.end()) {
    Log("Duplicated component added!");
    return;
  }
  _components.push_back(component);
}

void GameObject::setComponent(std::shared_ptr<Component> component) {
  for (auto &existingComponent : _components) {
    if (typeid(*&existingComponent) == typeid(*&component)) {
      existingComponent = component;
      return;
    }
  }
  addComponent(component);
}