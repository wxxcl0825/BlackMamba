#pragma once

#include "common/common.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"

namespace Layers
{
    static constexpr uint8_t UNUSED1 = 0; // 4 unused values so that broadphase layers values don't match with
                                              // object layer values (for testing purposes)
    static constexpr uint8_t UNUSED2    = 1;
    static constexpr uint8_t UNUSED3    = 2;
    static constexpr uint8_t UNUSED4    = 3;
    static constexpr uint8_t NON_MOVING = 4;
    static constexpr uint8_t MOVING     = 5;
    static constexpr uint8_t DEBRIS     = 6; // Example: Debris collides only with NON_MOVING
    static constexpr uint8_t SENSOR     = 7; // Sensors only collide with MOVING objects
    static constexpr uint8_t NUM_LAYERS = 8;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::BroadPhaseLayer DEBRIS(2);
    static constexpr JPH::BroadPhaseLayer SENSOR(3);
    static constexpr JPH::BroadPhaseLayer UNUSED(4);
    static constexpr uint32_t             NUM_LAYERS(5);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl();

    uint32_t GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {return "BPLayerInterfaceImpl";}

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        return _object_to_broad_phase[inLayer];
    }

private:
    JPH::BroadPhaseLayer _object_to_broad_phase[Layers::NUM_LAYERS];
};

class MyObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter{
public:
    bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter{
public:
    bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override;
};

inline JPH::Vec3 toVec3(glm::vec3 v) { return {v.x, v.y, v.z}; }
inline glm::vec3 toVec3(JPH::Vec3 v) { return {v.GetX(), v.GetY(), v.GetZ()}; }

inline JPH::Quat toQuat(glm::vec4 q) { return {q.x, q.y, q.z, q.w}; }
inline glm::vec4 toQuat(JPH::Quat q) { return {q.GetX(), q.GetY(), q.GetZ(), q.GetW()}; }

inline glm::vec4 toRotation(glm::vec3 angle){
    return {cos(angle.x / 2) * cos(angle.y / 2) * cos(angle.z / 2) + sin(angle.x / 2) * sin(angle.y / 2) * sin(angle.z / 2),
            sin(angle.x / 2) * cos(angle.y / 2) * cos(angle.z / 2) - cos(angle.x / 2) * sin(angle.y / 2) * sin(angle.z / 2),
            cos(angle.x / 2) * sin(angle.y / 2) * cos(angle.z / 2) + sin(angle.x / 2) * cos(angle.y / 2) * sin(angle.z / 2),
            cos(angle.x / 2) * cos(angle.y / 2) * sin(angle.z / 2) - sin(angle.x / 2) * sin(angle.y / 2) * cos(angle.z / 2)};
}
inline glm::vec3 toEuler(glm::vec4 q){
    return {atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y)),
            asin(2 * (q.w * q.y - q.z * q.x)),
            atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))};
}
