#pragma once

#include "common/common.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "glm/fwd.hpp"

namespace Layers
{
    static constexpr uint8_t STATIC     = 0;
    static constexpr uint8_t MOVING     = 1;
    static constexpr uint8_t NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr uint32_t             NUM_LAYERS(2);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl();

    uint32_t GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;

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
    glm::vec3 angle_rad = glm::radians(angle);
    return {sin(angle_rad.x / 2) * cos(angle_rad.y / 2) * cos(angle_rad.z / 2) - cos(angle_rad.x / 2) * sin(angle_rad.y / 2) * sin(angle_rad.z / 2),
            cos(angle_rad.x / 2) * sin(angle_rad.y / 2) * cos(angle_rad.z / 2) + sin(angle_rad.x / 2) * cos(angle_rad.y / 2) * sin(angle_rad.z / 2),
            cos(angle_rad.x / 2) * cos(angle_rad.y / 2) * sin(angle_rad.z / 2) - sin(angle_rad.x / 2) * sin(angle_rad.y / 2) * cos(angle_rad.z / 2),
            cos(angle_rad.x / 2) * cos(angle_rad.y / 2) * cos(angle_rad.z / 2) + sin(angle_rad.x / 2) * sin(angle_rad.y / 2) * sin(angle_rad.z / 2)};
}
inline glm::vec3 toEuler(glm::vec4 q){
    float pitch = asin(2 * (q.w * q.y - q.z * q.x));

    float yaw, roll;
    if (abs(pitch) < glm::pi<float>() / 2) {
        yaw = atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z));
        roll = atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y));
    } else {
        yaw = atan2(2 * (q.w * q.z - q.x * q.y), 1 - 2 * (q.x * q.x + q.z * q.z));
        roll = 0;
    }

    return glm::degrees(glm::vec3(roll, pitch, yaw));
}
