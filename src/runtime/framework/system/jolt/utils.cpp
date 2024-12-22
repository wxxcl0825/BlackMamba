#include "runtime/framework/system/jolt/utils.h"

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
    // Create a mapping table from object to broad phase layer
    _object_to_broad_phase[Layers::UNUSED1]    = BroadPhaseLayers::UNUSED;
    _object_to_broad_phase[Layers::UNUSED2]    = BroadPhaseLayers::UNUSED;
    _object_to_broad_phase[Layers::UNUSED3]    = BroadPhaseLayers::UNUSED;
    _object_to_broad_phase[Layers::UNUSED4]    = BroadPhaseLayers::UNUSED;
    _object_to_broad_phase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    _object_to_broad_phase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    _object_to_broad_phase[Layers::DEBRIS]     = BroadPhaseLayers::DEBRIS;
    _object_to_broad_phase[Layers::SENSOR]     = BroadPhaseLayers::SENSOR;
}

bool MyObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
    switch (inLayer1)
    {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING ||
                   inLayer2 == BroadPhaseLayers::SENSOR;
        case Layers::DEBRIS:
            return inLayer2 == BroadPhaseLayers::NON_MOVING;
        case Layers::SENSOR:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::UNUSED1:
        case Layers::UNUSED2:
        case Layers::UNUSED3:
            return false;
        default:
            return false;
    }
}

bool MyObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
    switch (inObject1)
    {
        case Layers::UNUSED1:
        case Layers::UNUSED2:
        case Layers::UNUSED3:
        case Layers::UNUSED4:
            return false;
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING || inObject2 == Layers::DEBRIS;
        case Layers::MOVING:
            return inObject2 == Layers::NON_MOVING || inObject2 == Layers::MOVING || inObject2 == Layers::SENSOR;
        case Layers::DEBRIS:
            return inObject2 == Layers::NON_MOVING;
        case Layers::SENSOR:
            return inObject2 == Layers::MOVING;
        default:
            return false;
    }
}
