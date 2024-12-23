#include "runtime/framework/system/physicalSystem.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "common/macro.h"
#include "runtime/framework/component/physics/rigidBody.h"
#include "runtime/framework/component/transform/transform.h"

void PhysicalSystem::unregisterComponent(uint32_t id) {
    destroyRigidBody(id);
}

bool PhysicalSystem::init(PhysicsInfo info) {
    // init physics config
    _config = info;

    // init jolt physics
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
 
    _joltPhysics._joltPhysicsSystem = new JPH::PhysicsSystem();
    _joltPhysics._joltBroadPhaseLayerInterface = new BPLayerInterfaceImpl();

    _joltPhysics._joltJobSystem = new JPH::JobSystemThreadPool(_config.maxJobCount,
                                         _config.maxBarrierCount,
                                         static_cast<int>(_config.maxConcurrentJobCount));

    // 16M temp memory
    _joltPhysics._tempAllocator = new JPH::TempAllocatorImpl(16 * 1024 * 1024);
    
    _joltPhysics._objectVsBroadPhaseFilter = new MyObjectVsBroadPhaseLayerFilter();
    _joltPhysics._objectLayerPairFilter = new MyObjectLayerPairFilter();
    _joltPhysics._joltPhysicsSystem->Init(   _config.maxBodyCount,
                                                _config.bodyMutexCount, 
                                                _config.maxBodyPairs, 
                                                _config.maxContactConstraints, 
                                                *(_joltPhysics._joltBroadPhaseLayerInterface),
                                                *(_joltPhysics._objectVsBroadPhaseFilter),
                                                *(_joltPhysics._objectLayerPairFilter));

    // _joltPhysics._joltPhysicsSystem->SetPhysicsSettings(JPH::PhysicsSettings());
    _joltPhysics._joltPhysicsSystem->SetGravity({_config.gravity.x, _config.gravity.y, _config.gravity.z});

    return true;
}

void PhysicalSystem::dispatch(GameObject *object) {
    std::shared_ptr<RigidBodyComponent> _rigidBody = object->getComponent<RigidBodyComponent>();
    if (_rigidBody) {
        std::shared_ptr<TransformComponent> _transform = object->getComponent<TransformComponent>();
        if(_rigidBody->getId() == 0xffffffff) {
            createRigidBody(object);
        }
        else if(_rigidBody->getMotionType() == JPH::EMotionType::Dynamic) {
            // update rigid body
            JPH::BodyInterface& bodyInterface = _joltPhysics._joltPhysicsSystem->GetBodyInterface();
            JPH::Vec3 position;
            JPH::Quat rotation;

            bodyInterface.GetPositionAndRotation(JPH::BodyID(_rigidBody->getId()), position, rotation);
            _transform->setPositionLocal(glm::inverse(_transform->getParentModel()) * glm::vec4(toVec3(position), 1.0));
            _transform->setAngle(toEuler(toQuat(rotation)));

            // apply force
            glm::vec3 force = _rigidBody->getForce();
            glm::vec3 torque = _rigidBody->getTorque();
            JPH::Vec3 linearVelocity = bodyInterface.GetLinearVelocity(JPH::BodyID(_rigidBody->getId()));
            JPH::Vec3 angularVelocity = bodyInterface.GetAngularVelocity(JPH::BodyID(_rigidBody->getId()));

            bodyInterface.AddForce(JPH::BodyID(_rigidBody->getId()), toVec3(force));
            bodyInterface.AddTorque(JPH::BodyID(_rigidBody->getId()), toVec3(torque));
            // limit speed
            if(linearVelocity.Length() > _rigidBody->getMaxLinearVelocity()) {
                bodyInterface.SetLinearVelocity(JPH::BodyID(_rigidBody->getId()), linearVelocity.Normalized() * _rigidBody->getMaxLinearVelocity());
            }
            if(angularVelocity.Length() > _rigidBody->getMaxAngularVelocity()) {
                bodyInterface.SetAngularVelocity(JPH::BodyID(_rigidBody->getId()), angularVelocity.Normalized() * _rigidBody->getMaxAngularVelocity());
            }

            _rigidBody->setLinearVelocity(toVec3(linearVelocity));
        }
    }
}

void PhysicalSystem::tick(float dt) {

    const float deltaTime = 1.0f / _config.updateFrequency;
    _joltPhysics._joltPhysicsSystem->Update(deltaTime,
                                            _joltPhysics._collisionSteps,
                                            _joltPhysics._tempAllocator,
                                            _joltPhysics._joltJobSystem);

    clear();
}

void PhysicalSystem::clear() {
    _physics.clear();
}

uint32_t PhysicalSystem::createRigidBody(GameObject *object) {
    std::shared_ptr<RigidBodyComponent> _rigidBody = object->getComponent<RigidBodyComponent>();
    std::shared_ptr<TransformComponent> _transform = object->getComponent<TransformComponent>();

    glm::vec3 position  = _transform->getPositionWorld();
    glm::vec3 scale     = {1.0f, 1.0f, 1.0f};
    glm::vec3 angle     = _transform->getAngle();

    JPH::ShapeRefC   shape      = _rigidBody->getShape();
    JPH::EMotionType motionType = _rigidBody->getMotionType();
    JPH::ObjectLayer layer      = _rigidBody->getLayer();

    JPH::BodyCreationSettings settings(shape, toVec3(position), toQuat(toRotation(angle)), motionType, layer);
    settings.mApplyGyroscopicForce  = true;
    settings.mMaxLinearVelocity     = 10000.0;
    settings.mLinearDamping         = 0.5;
    settings.mAngularDamping        = 0.5;

    JPH::BodyInterface& bodyInterface = _joltPhysics._joltPhysicsSystem->GetBodyInterface();
    JPH::Body *body = bodyInterface.CreateBody(settings);
    // body->SetFriction(0.5);
    // form 0.0 to 1.0
    // body->SetRestitution(0.3f);
    bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

    uint32_t id = body->GetID().GetIndexAndSequenceNumber();
    _rigidBody->setId(id);

    return id;
}

void PhysicalSystem::destroyRigidBody(uint32_t ridigbodyId) {
    // destroy rigid body
    JPH::BodyInterface& bodyInterface = _joltPhysics._joltPhysicsSystem->GetBodyInterface();
    bodyInterface.RemoveBody(JPH::BodyID(ridigbodyId));
    // bodyInterface.DestroyBody(JPH::BodyID(ridigbodyId));
}