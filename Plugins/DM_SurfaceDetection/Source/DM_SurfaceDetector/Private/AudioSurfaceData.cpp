#include "AudioSurfaceData.h"

bool UAudioSurfaceData::IsValidSurfaceType(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
    return SurfaceTypeMap.Contains(SurfaceType);
}

int32 UAudioSurfaceData::GetMetasoundParameter(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
    const int32* FoundValue = SurfaceTypeMap.Find(SurfaceType);
    if (FoundValue)
    {
        return *FoundValue;
    }

    return -1;
}

#if WITH_EDITOR
void UAudioSurfaceData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // No validation needed - SurfaceType_Default is now allowed in the map
}
#endif
