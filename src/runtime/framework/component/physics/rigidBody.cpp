#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "runtime/framework/component/physics/rigidBody.h"

// for boxs
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, uint32_t x, uint32_t y, uint32_t z, float density) 
        : _motionType(motion_type), _layer(layer), _density(density)
{ 
    JPH::BoxShapeSettings body_shape_settings(JPH::Vec3(0.5 * x, 0.5 * y, 0.5 * z));
    body_shape_settings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
}

// for spheres
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, float radius, float density)
      : _motionType(motion_type), _layer(layer), _density(density)
{ 
    JPH::SphereShapeSettings body_shape_settings(radius);
    body_shape_settings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
}

// for capsules
RigidBodyComponent::RigidBodyComponent(JPH::EMotionType motion_type, JPH::ObjectLayer layer, float radius, float half_height, float density)
      : _motionType(motion_type), _layer(layer), _density(density)
{
    JPH::CapsuleShapeSettings body_shape_settings(half_height,radius);
    body_shape_settings.SetDensity(_density);

    JPH::ShapeSettings::ShapeResult body_shape_result = body_shape_settings.Create();
    _shape = body_shape_result.Get();
};