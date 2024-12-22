#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/COre/TempAllocator.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "runtime/framework/system/jolt/utils.h"
#include "runtime/framework/object/gameObject.h"

#define MAXPHTSICSOBJECTS 1024

struct PhysicsInfo {
    // scene setting
    uint32_t max_body_count;
    uint32_t body_mutex_count;
    uint32_t max_body_pairs;
    uint32_t max_contact_constraints;

    // job setting
    uint32_t max_job_count;
    uint32_t max_barrier_count;
    uint32_t max_concurrent_job_count;

    glm::vec3 gravity;

    float update_frequency;
};

struct PhysicsConfig
{
    uint32_t _max_body_count {10240};
    uint32_t _body_mutex_count {0} ;
    uint32_t _max_body_pairs {65536};
    uint32_t _max_contact_constraints {10240};

    uint32_t _max_job_count {1024};
    uint32_t _max_barrier_count {8};
    uint32_t _max_concurrent_job_count {4};

    glm::vec3 _gravity {0.f, 0.f, -9.8f};

    float _update_frequency {60.f};
};

class PhysicalSystem{
    struct JoltPhysics
    {
        JPH::PhysicsSystem*            _jolt_physics_system {nullptr};
        JPH::JobSystem*                _jolt_job_system {nullptr};
        JPH::TempAllocator*            _temp_allocator {nullptr};
        JPH::BroadPhaseLayerInterface* _jolt_broad_phase_layer_interface {nullptr};

        MyObjectVsBroadPhaseLayerFilter *object_vs_broad_phase_filter {nullptr};
        MyObjectLayerPairFilter *object_layer_pair_filter {nullptr};

        int _collision_steps {1};
    };

public:
    PhysicalSystem() = default;
    ~PhysicalSystem() = default;

    glm::vec3 getGravity() const;

    uint32_t registerComponent();
    void unregisterComponent(uint32_t id);

    bool init(PhysicsInfo info);
    void dispatch(GameObject* object);
    void tick(float dt);

private:
    uint32_t _next_id{0};
    std::unordered_map<uint32_t, uint32_t> _id_to_body;
    std::vector<GameObject *> _physics;

    PhysicsConfig _config;
    JoltPhysics _jolt_physics;

    uint32_t createRigidBody(GameObject *object);
    void destroyRigidBody(uint32_t bodyId);
    void updateRigidBody(GameObject *object);

    void clear();
};