#include "runtime/framework/object/gameObject.h"

GameObject::~GameObject() {
  for (auto component : _components)
    if (component) {
      delete component;
      component = nullptr;
    }
  for (auto child : _children)
    if (child) {
      delete child;
      child = nullptr;
    }
}

void GameObject::addChild(GameObject *object) {
  object->_parent = this;
  _children.push_back(object);
}

void GameObject::addComponent(Component *component) {
  _components.push_back(component);
}