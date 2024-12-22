#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "runtime/framework/component/physics/rigidBody.h"

// for boxs
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, uint32_t x, uint32_t y, uint32_t z, float density) 
        : _motionType(motionType), _layer(layer), _density(density)
{ 
    JPH::BoxShapeSettings bodyShapeSettings(JPH::Vec3(0.5 * x, 0.5 * y, 0.5 * z));
    bodyShapeSettings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
    _shape = bodyShapeResult.Get();
}

// for spheres
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float density)
      : _motionType(motionType), _layer(layer), _density(density)
{ 
    JPH::SphereShapeSettings bodyShapeSettings(radius);
    bodyShapeSettings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
    _shape = bodyShapeResult.Get();
}

// for capsules
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motionType, JPH::ObjectLayer layer, float radius, float halfHeight, float density)
      : _motionType(motionType), _layer(layer), _density(density)
{
    JPH::CapsuleShapeSettings bodyShapeSettings(halfHeight,radius);
    bodyShapeSettings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
    _shape = bodyShapeResult.Get();
};