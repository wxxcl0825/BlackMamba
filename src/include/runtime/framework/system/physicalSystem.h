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
    uint32_t maxBodyCount;
    uint32_t bodyMutexCount;
    uint32_t maxBodyPairs;
    uint32_t maxContactConstraints;

    // job setting
    uint32_t maxJobCount;
    uint32_t maxBarrierCount;
    uint32_t maxConcurrentJobCount;

    glm::vec3 gravity;

    float updateFrequency;
    int collisionSteps;
};

class PhysicalSystem{
    struct JoltPhysics
    {
        JPH::PhysicsSystem*            _joltPhysicsSystem {nullptr};
        JPH::JobSystem*                _joltJobSystem {nullptr};
        JPH::TempAllocator*            _tempAllocator {nullptr};
        JPH::BroadPhaseLayerInterface* _joltBroadPhaseLayerInterface {nullptr};

        MyObjectVsBroadPhaseLayerFilter *_objectVsBroadPhaseFilter {nullptr};
        MyObjectLayerPairFilter *_objectLayerPairFilter {nullptr};

        int _collisionSteps {1};
    };

public:
    PhysicalSystem() = default;
    ~PhysicalSystem() = default;

    glm::vec3 getGravity() const;

    void unregisterComponent(uint32_t id);

    bool init(PhysicsInfo info);
    void dispatch(GameObject* object);
    void tick(float dt);

private:
    std::vector<GameObject *> _physics;

    PhysicsInfo _config;
    JoltPhysics _joltPhysics;

    uint32_t createRigidBody(GameObject *object);
    void destroyRigidBody(uint32_t bodyId);
    void updateRigidBody(GameObject *object);

    void clear();
};