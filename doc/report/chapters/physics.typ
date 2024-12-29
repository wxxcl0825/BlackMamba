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

= 物理系统

物理系统是游戏引擎中的重要组成部分，用于模拟物体的运动和碰撞效果。每次物理系统调用tick时，根据其在物理世界中的位置、速度、加速度等参数来更新物体的位置和速度，从而模拟物体的运动。物理系统还可以检测物体之间的碰撞，当两个物体发生碰撞时，会产生相互作用力，从而改变物体的运动状态。

== 物理组件

物理组件是游戏中用于模拟物体运动和碰撞效果的组件，其主要功能是设置物体的位置、速度、碰撞箱、物理id(0xffffffff为无效值)等参数。本项目中，物理组件实现了Box，Sphere和Capsule三种碰撞箱以及自定义碰撞箱的初始化。物理组件声明如下：

#H(title: "include/runtime/framework/component/physics/rigidBody.h")

```cpp
class RigidBodyComponent : public Component {
public:
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, JPH::ShapeRefC shape)
      : _motionType(motionType), _layer(layer), _shape(shape) {};
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, uint32_t x, uint32_t y, uint32_t z, float density);
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float density);
  RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float halfHeight, float density);

  ~RigidBodyComponent() override{Engine::getEngine()->getPhysicalSystem()-> unregisterComponent(_id);};

  uint32_t getId() const { return _id; }
  float getDensity() const {return _density;}
  float getLiftCoefficient() const {return _liftCoefficient;}
  JPH::EMotionType getMotionType() const { return _motionType; }
  JPH::ObjectLayer getLayer() const { return _layer; }
  JPH::ShapeRefC getShape() const { return _shape; }
  glm::vec3 getForce() const { return _force; }
  glm::vec3 getTorque() const { return _torque; }
  glm::vec3 getLinearVelocity() const { return _linearVelocity; }
  float getMaxLinearVelocity() const { return _maxLinearVelocity; }
  float getMaxAngularVelocity() const { return _maxAngularVelocity; }
  bool isCollide() const { return _isCollide; }

  void setId(uint32_t id) { _id = id; }
  void setDensity(float density) { _density = density; }
  void setLiftCoefficient(float liftCoefficient) { _liftCoefficient = liftCoefficient; }
  void setMotionType(JPH::EMotionType type) { _motionType = type; }
  void setLayer(JPH::ObjectLayer layer) { this->_layer = layer; }
  void setShape(JPH::Shape* shape) { this->_shape = shape; }
  void setForce(glm::vec3 force) { _force = force; }
  void setTorque(glm::vec3 torque) { _torque = torque; }
  void setLinearVelocity(glm::vec3 linearVelocity) { _linearVelocity = linearVelocity; }
  void setMaxLinearVelocity(float maxLinearVelocity) { _maxLinearVelocity = maxLinearVelocity; }
  void setMaxAngularVelocity(float maxAngularVelocity) { _maxAngularVelocity = maxAngularVelocity; }
  void setCollide(bool isCollide) { _isCollide = isCollide; }

private:
  uint32_t _id {0xffffffff};

  float _density {1.0f};
  float _liftCoefficient {0.5f};
  JPH::EMotionType _motionType = JPH::EMotionType::Static;
  JPH::ObjectLayer _layer       = Layers::STATIC;
  JPH::ShapeRefC _shape {nullptr};

  glm::vec3 _force {0.0f, 0.0f, 0.0f};
  glm::vec3 _torque {0.0f, 0.0f, 0.0f};
  glm::vec3 _linearVelocity {0.0f, 0.0f, 0.0f};
  float _maxLinearVelocity {500.0f};
  float _maxAngularVelocity {0.25f * JPH::JPH_PI};

  bool _isCollide {false};
};
```
- 使用Jolt Physics库@joltphysics 作为力学模拟的后端

#colbreak()

其初始化时，可以通过传入不同的参数来创建不同形状的碰撞箱，这些函数的实现如下：

#H(title: "runtime/framework/component/physics/rigidBody.cpp")

```cpp
// for boxs
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType,
                                       JPH::ObjectLayer layer, uint32_t x,
                                       uint32_t y, uint32_t z, float density)
    : _motionType(motionType), _layer(layer), _density(density) {
  JPH::BoxShapeSettings bodyShapeSettings(JPH::Vec3(0.5 * x, 0.5 * y, 0.5 * z));
  bodyShapeSettings.SetDensity(_density);

  JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
  _shape = bodyShapeResult.Get();
}

// for spheres
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType,
                                       JPH::ObjectLayer layer, float radius,
                                       float density)
    : _motionType(motionType), _layer(layer), _density(density) {
  JPH::SphereShapeSettings bodyShapeSettings(radius);
  bodyShapeSettings.SetDensity(_density);

  JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
  _shape = bodyShapeResult.Get();
}

// for capsules
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType,
                                       JPH::ObjectLayer layer, float radius,
                                       float halfHeight, float density)
    : _motionType(motionType), _layer(layer), _density(density) {
  JPH::CapsuleShapeSettings bodyShapeSettings(halfHeight, radius);
  bodyShapeSettings.SetDensity(_density);

  JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
  _shape = bodyShapeResult.Get();
};
```

#colbreak()

== 碰撞检测

=== 碰撞箱
物理系统中，碰撞箱是一个用于检测碰撞的简单几何体，例如立方体、球体、胶囊体等，其形状和大小可以根据需要进行调整。

#figure(image("../images/4.1.png", width: 80%), caption: "碰撞箱，图片来源" + [@games104])

=== 碰撞检测算法

==== Board-Phase
此阶段主要是为了减少碰撞检测的次数，将所有的碰撞箱分为不同的组，每个组中的碰撞箱都是不会发生碰撞的，这样就可以减少碰撞检测的次数。接着，通过AABB算法对每个组中的碰撞箱进行碰撞检测。AABB算法是一种简单的碰撞检测算法，以碰撞箱的包围盒为基础，通过检测包围盒之间是否相交来判断碰撞箱是否发生碰撞。

以二维碰撞箱为例，碰撞箱的包围盒是一个矩形，其左上角坐标为$(x_1, y_1)$，右下角坐标为$(x_2, y_2)$. 如果两个碰撞箱的包围盒相交，则说明两个碰撞箱可能发生碰撞，接下来进入Narrow-Phase阶段进行精确的碰撞检测。

==== Narrow-Phase

在Narrow-Phase阶段，对Board-Phase阶段中可能发生碰撞的碰撞箱进行精确的碰撞检测。对于不同形状的碰撞箱，可以采用不同的碰撞检测算法，例如，对于球体和球体之间的碰撞检测，可以采用球体之间的距离公式来判断是否发生碰撞；对于球体和立方体之间的碰撞检测，可以采用球心到立方体表面的距离来判断是否发生碰撞。这些碰撞的检测算法较为直白，运算量较大但可以获得精细的参数。

#figure(image("../images/4.2.png", width: 34%), caption: "碰撞检测算法，图片来源" + [@games104])

== 力学模拟

=== 刚体
在物理学中，刚体是指形状不变的物体，即物体的形状和大小在运动过程中不会发生变化。在游戏中，刚体是一个用于模拟物体运动的基本单位，其运动状态由位置、速度、角度和角速度等参数来描述。

=== 质心定理

质心定理是刚体运动学中的一个重要定理，刚体的运动可以看作是质心的平动和绕质心的转动的叠加。
  
$
  bold(v)_i = bold(v)_"CM"+bold(omega) times bold(r)_i
$
  
在游戏中，质心定理可以用来简化刚体的运动模拟，将刚体的运动分解为质心的平动和绕质心的转动两个部分，从而简化运动方程的求解。

=== 物体运动定律

物体运动状态的改变是由外力引起的，一个力作用在物体上时，首先会对物体位置的运动产生作用，其等效于对物体的质心施加一个力，从而改变物体的位置；其次会对物体的角度运动产生作用，其等效于对物体的质心施加一个力矩，从而改变物体的角度。
  
$
  bold(F)_"ext"=M (d^2 bold(R)_"CM")/(d t^2)\
  bold(M)_"ext"=(d(bold(I)bold(omega)))/(d t)
$
  
结合质心定理，可以得到物体的运动方程，从而模拟物体的运动。

=== 碰撞
在游戏中，当两个物体发生碰撞时，两个物体之间会产生相互作用力，从而改变物体的运动状态。相互作用力的大小和方向可以根据碰撞箱的形状和碰撞箱之间的相对位置来计算，从而模拟物体的碰撞效果。

#colbreak()

== Jolt Physics@joltphysics

JoltPhysics是一款基于C++的物理引擎，其主要功能是模拟物体的运动和碰撞效果。JoltPhysics提供了丰富的API接口，可以方便地创建物体、设置物体的运动参数、检测碰撞等。JoltPhysics还提供了丰富的碰撞检测算法和碰撞效果，可以满足不同类型游戏的需求。

=== 物理系统初始化

在使用JoltPhysics时，需要初始化物理系统，包括创建物理世界、设置重力加速度等。此时注意，初始化时需要实现`BPLayerInterfaceImpl`类，其中需要自定义`Layers`层和`BroadPhaseLayers`层(`Layers`用于精确控制物体之间的碰撞规则，允许物体之间的碰撞选择性发生，`BroadPhaseLayers` 主要用于碰撞检测的初步筛选，帮助快速排除不可能发生碰撞的物体，从而加速模拟过程)，以及实现`MyObjectVsBroadPhaseLayerFilter`类和`MyObjectLayerPairFilter`类，用于控制物体之间的可碰撞关系。
  
其声明与实现如下：

#H(title: "include/runtime/framework/system/jolt/utils.h")

```cpp
namespace Layers {
static constexpr uint8_t STATIC = 0;
static constexpr uint8_t MOVING = 1;
static constexpr uint8_t NUM_LAYERS = 2;
}; // namespace Layers

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr uint32_t NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
  BPLayerInterfaceImpl();

  uint32_t GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::NUM_LAYERS;
  }

  const char *
  GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;

  JPH::BroadPhaseLayer
  GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
    return _object_to_broad_phase[inLayer];
  }

private:
  JPH::BroadPhaseLayer _object_to_broad_phase[Layers::NUM_LAYERS];
};

class MyObjectVsBroadPhaseLayerFilter
    : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
  bool ShouldCollide(JPH::ObjectLayer inLayer1,
                     JPH::BroadPhaseLayer inLayer2) const override;
};

class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
  bool ShouldCollide(JPH::ObjectLayer inLayer1,
                     JPH::ObjectLayer inLayer2) const override;
};
```

#H(title: "runtime/framework/system/jolt/utils.cpp")
```cpp
BPLayerInterfaceImpl::BPLayerInterfaceImpl() {
  // Create a mapping table from object to broad phase layer
  _object_to_broad_phase[Layers::STATIC] = BroadPhaseLayers::NON_MOVING;
  _object_to_broad_phase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

const char *BPLayerInterfaceImpl::GetBroadPhaseLayerName(
    JPH::BroadPhaseLayer inLayer) const {
  switch ((JPH::BroadPhaseLayer::Type)inLayer) {
  case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
    return "NON_MOVING";
  case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
    return "MOVING";
  default:
    return "INVALID";
  }
}

bool MyObjectVsBroadPhaseLayerFilter::ShouldCollide(
    JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const {
  switch (inLayer1) {
  case Layers::STATIC:
    return inLayer2 == BroadPhaseLayers::MOVING;
  case Layers::MOVING:
    return inLayer2 == BroadPhaseLayers::NON_MOVING ||
           inLayer2 == BroadPhaseLayers::MOVING;
  default:
    return false;
  }
}

bool MyObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inObject1,
                                            JPH::ObjectLayer inObject2) const {
  switch (inObject1) {
  case Layers::STATIC:
    return inObject2 == Layers::MOVING;
  case Layers::MOVING:
    return inObject2 == Layers::STATIC || inObject2 == Layers::MOVING;
  default:
    return false;
  }
}
```

实现基本类后，其初始化代码如下：

#H(title: "runtime/framework/system/physicalSystem.cpp")

```cpp
bool PhysicalSystem::init(PhysicsInfo info) {
  // init physics config
  _config = info;

  // init jolt physics
  JPH::RegisterDefaultAllocator();
  JPH::Factory::sInstance = new JPH::Factory();
  JPH::RegisterTypes();

  _joltPhysics._joltPhysicsSystem = new JPH::PhysicsSystem();
  _joltPhysics._joltBroadPhaseLayerInterface = new BPLayerInterfaceImpl();

  _joltPhysics._joltJobSystem = new JPH::JobSystemThreadPool(
      _config.maxJobCount, _config.maxBarrierCount,
      static_cast<int>(_config.maxConcurrentJobCount));

  // 16M temp memory
  _joltPhysics._tempAllocator = new JPH::TempAllocatorImpl(16 * 1024 * 1024);

  _joltPhysics._objectVsBroadPhaseFilter =
      new MyObjectVsBroadPhaseLayerFilter();
  _joltPhysics._objectLayerPairFilter = new MyObjectLayerPairFilter();
  _joltPhysics._joltPhysicsSystem->Init(
      _config.maxBodyCount, _config.bodyMutexCount, _config.maxBodyPairs,
      _config.maxContactConstraints,
      *(_joltPhysics._joltBroadPhaseLayerInterface),
      *(_joltPhysics._objectVsBroadPhaseFilter),
      *(_joltPhysics._objectLayerPairFilter));

  _joltPhysics._contactListener = new MyContactListener();
  _joltPhysics._joltPhysicsSystem->SetContactListener(
      _joltPhysics._contactListener);

  _joltPhysics._joltPhysicsSystem->SetGravity(
      {_config.gravity.x, _config.gravity.y, _config.gravity.z});

  return true;
}
```

物理系统初始化完成后，就可以开始模拟物体的运动和碰撞效果了。

=== 物体放置与移除

物体放置是指在物理世界中放置物体，并设置对应的位置、速度、角度等参数，同时注意设置物体的碰撞箱、质量和惯性等参数。在JoltPhysics中，通过`BodyInterface`设置，其代码如下：

#H(title: "runtime/framework/system/physicalSystem.cpp")

```cpp
uint32_t PhysicalSystem::createRigidBody(GameObject *object) {
  std::shared_ptr<RigidBodyComponent> _rigidBody =
      object->getComponent<RigidBodyComponent>();
  std::shared_ptr<TransformComponent> _transform =
      object->getComponent<TransformComponent>();

  glm::vec3 position = _transform->getPositionWorld();
  glm::vec3 scale = {1.0f, 1.0f, 1.0f};
  glm::vec3 angle = _transform->getAngle();

  JPH::ShapeRefC shape = _rigidBody->getShape();
  JPH::EMotionType motionType = _rigidBody->getMotionType();
  JPH::ObjectLayer layer = _rigidBody->getLayer();

  JPH::BodyCreationSettings settings(
      shape, toVec3(position), toQuat(toRotation(angle)), motionType, layer);
  settings.mApplyGyroscopicForce = true;
  settings.mMaxLinearVelocity = 10000.0;
  settings.mLinearDamping = 0.1;
  settings.mAngularDamping = 0.1;

  JPH::BodyInterface &bodyInterface =
      _joltPhysics._joltPhysicsSystem->GetBodyInterface();
  JPH::Body *body = bodyInterface.CreateBody(settings);
  bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);
  _joltPhysics._contactListener->registerBody(
      body->GetID().GetIndexAndSequenceNumber());

  uint32_t id = body->GetID().GetIndexAndSequenceNumber();
  _rigidBody->setId(id);

  return id;
}
```

物体的移除也是类似的，通过`BodyInterface`的`RemoveBody`函数来移除物体，其代码如下：

#H(title: "runtime/framework/system/physicalSystem.cpp")

```cpp
void PhysicalSystem::destroyRigidBody(uint32_t ridigbodyId) {
  JPH::BodyInterface &bodyInterface =
      _joltPhysics._joltPhysicsSystem->GetBodyInterface();
  bodyInterface.RemoveBody(JPH::BodyID(ridigbodyId));
  // bodyInterface.DestroyBody(JPH::BodyID(ridigbodyId));
}
```
- 在最新版本的Jolt中，`RemoveBody`后无法立即调用`DestroyBody`, 由于此处未做精细的线程管理，跳过了`DestroyBody`的调用，可能会造成一定的内存泄漏和资源占用

=== 物理系统更新

每一帧中，调用物理系统的`tick`函数，根据物体的位置、速度、加速度等参数来更新物体的位置和速度，从而模拟物体的运动。在更新物理系统时，首先，根据物体的位置、速度、加速度等参数来更新物体的位置和速度，从而模拟物体的运动；其次，物理系统会检测物体之间的碰撞，当两个物体发生碰撞时，会产生相互作用力，从而改变物体的运动状态。
  
物理系统更新的代码如下：

#H(title: "runtime/framework/system/physicalSystem.cpp")

```cpp
void PhysicalSystem::update(float dt) {
  // update physics
  _joltPhysics._joltPhysicsSystem->Update(dt);
}
```

物理系统更新完成后，就可以模拟出物体的运动和碰撞效果了。