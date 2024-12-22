#include "runtime/framework/system/physicalSystem.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <cstdint>

#include "glm/fwd.hpp"
#include "runtime/framework/component/physics/rigidBody.h"
#include "runtime/framework/component/transform/transform.h"

uint32_t PhysicalSystem::registerComponent() {
    return _next_id++;
}

void PhysicalSystem::unregisterComponent(uint32_t id) {
    uint32_t ridigbodyId = _id_to_body[id];
    destroyRigidBody(ridigbodyId);
    _id_to_body.erase(id);
}

bool PhysicalSystem::init(PhysicsInfo info) {
    // init physics config
    _config._max_body_count = info.max_body_count;
    _config._body_mutex_count = info.body_mutex_count;
    _config._max_body_pairs = info.max_body_pairs;
    _config._max_contact_constraints = info.max_contact_constraints;
    _config._max_job_count = info.max_job_count;
    _config._max_barrier_count = info.max_barrier_count;
    _config._max_concurrent_job_count = info.max_concurrent_job_count;
    _config._gravity = info.gravity;
    _config._update_frequency = info.update_frequency;

    // init jolt physics
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
 
    _jolt_physics._jolt_physics_system = new JPH::PhysicsSystem();
    _jolt_physics._jolt_broad_phase_layer_interface = new BPLayerInterfaceImpl();

    _jolt_physics._jolt_job_system = new JPH::JobSystemThreadPool(_config._max_job_count,
                                         _config._max_barrier_count,
                                         static_cast<int>(_config._max_concurrent_job_count));

    // 16M temp memory
    _jolt_physics._temp_allocator = new JPH::TempAllocatorImpl(16 * 1024 * 1024);
    
    _jolt_physics.object_vs_broad_phase_filter = new MyObjectVsBroadPhaseLayerFilter();
    _jolt_physics.object_layer_pair_filter = new MyObjectLayerPairFilter();

    _jolt_physics._jolt_physics_system->Init(   _config._max_body_count,
                                                _config._body_mutex_count, 
                                                _config._max_body_pairs, 
                                                _config._max_contact_constraints, 
                                                *(_jolt_physics._jolt_broad_phase_layer_interface),
                                                *(_jolt_physics.object_vs_broad_phase_filter),
                                                *(_jolt_physics.object_layer_pair_filter));
    _jolt_physics._jolt_physics_system->SetPhysicsSettings(JPH::PhysicsSettings());
    _jolt_physics._jolt_physics_system->SetGravity({_config._gravity.x, _config._gravity.y, _config._gravity.z});

    return true;
}

void PhysicalSystem::dispatch(GameObject *object) {
    if (object->getComponent<RigidBodyComponent>()) {
        if(object->getComponent<RigidBodyComponent>()->getRigidBodyId() == 0xffffffff) {
            createRigidBody(object);
        }
        else{
            // update rigid body
            JPH::BodyInterface& body_interface = _jolt_physics._jolt_physics_system->GetBodyInterface();
            JPH::Vec3 position;
            JPH::Quat rotation;

            body_interface.GetPositionAndRotation(JPH::BodyID(object->getComponent<RigidBodyComponent>()->getRigidBodyId()), position, rotation);
            object->getComponent<TransformComponent>()->setPositionLocal(glm::inverse(object->getComponent<TransformComponent>()->getParentModel()) * glm::vec4(toVec3(position), 1.0));
            object->getComponent<TransformComponent>()->setAngle(toEuler(toQuat(rotation)));

            // apply force
            glm::vec3 force = object->getComponent<RigidBodyComponent>()->getForce();
            if(force != glm::vec3(0.0f, 0.0f, 0.0f)){
                body_interface.AddForce(JPH::BodyID(object->getComponent<RigidBodyComponent>()->getRigidBodyId()), toVec3(force), toVec3(glm::vec3(0.0f, 0.0f, 0.0f)));
            }

        }
    }
}

void PhysicalSystem::tick(float dt) {

    const float deltaTime = 1.0f / _config._update_frequency;
    _jolt_physics._jolt_physics_system->Update(deltaTime,
                                            _jolt_physics._collision_steps,
                                            _jolt_physics._temp_allocator,
                                            _jolt_physics._jolt_job_system);

    clear();
}

void PhysicalSystem::clear() {
    _physics.clear();
}

uint32_t PhysicalSystem::createRigidBody(GameObject *object) {
    glm::vec3 position = object->getComponent<TransformComponent>()->getPositionWorld();
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 angle = object->getComponent<TransformComponent>()->getAngle();
    glm::vec4 rotation = toRotation(angle);

    JPH::BodyInterface& body_interface = _jolt_physics._jolt_physics_system->GetBodyInterface();

    JPH::ShapeRefC shape = object->getComponent<RigidBodyComponent>()->getShape();

    JPH::EMotionType motion_type = object->getComponent<RigidBodyComponent>()->getMotionType();
    JPH::ObjectLayer layer       = object->getComponent<RigidBodyComponent>()->getLayer();
    JPH::BodyCreationSettings settings(shape, toVec3(position), toQuat(rotation), motion_type, layer);
    JPH::Body *body = body_interface.CreateBody(settings);

    body_interface.AddBody(body->GetID(), JPH::EActivation::Activate);
    object->getComponent<RigidBodyComponent>()->setRigidBodyId(body->GetID().GetIndex());

    return body->GetID().GetIndex();
}

void PhysicalSystem::destroyRigidBody(uint32_t ridigbodyId) {
    // destroy rigid body
    JPH::BodyInterface& bodyInterface = _jolt_physics._jolt_physics_system->GetBodyInterface();
    bodyInterface.RemoveBody(JPH::BodyID(ridigbodyId));
    bodyInterface.DestroyBody(JPH::BodyID(ridigbodyId));
}