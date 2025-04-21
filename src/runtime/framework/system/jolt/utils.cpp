#include "runtime/framework/system/jolt/utils.h"

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
    // Create a mapping table from object to broad phase layer
    _object_to_broad_phase[Layers::STATIC]    = BroadPhaseLayers::NON_MOVING;
    _object_to_broad_phase[Layers::MOVING]    = BroadPhaseLayers::MOVING;
}

const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const{
    switch ((JPH::BroadPhaseLayer::Type)inLayer)
    {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            return "INVALID";
    }
}

bool MyObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
    switch (inLayer1)
    {
        case Layers::STATIC:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING;
        default:
            return false;
    }
}

bool MyObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
    switch (inObject1)
    {
        case Layers::STATIC:
            return inObject2 == Layers::MOVING;
        case Layers::MOVING:
            return inObject2 == Layers::STATIC || inObject2 == Layers::MOVING;
        default:
            return false;
    }
}
