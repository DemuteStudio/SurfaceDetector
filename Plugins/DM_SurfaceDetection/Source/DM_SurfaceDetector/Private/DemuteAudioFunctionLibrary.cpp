#include "DemuteAudioFunctionLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInterface.h"

bool UDemuteAudioFunctionLibrary::LineTraceForSurfaceTypes(
    UObject* WorldContextObject,
    const FVector& Start,
    const FVector& End,
    ETraceTypeQuery TraceChannel,
    bool bTraceComplex,
    const TArray<AActor*>& ActorsToIgnore,
    UAudioSurfaceData* SurfaceData,
    int32& OutMetasoundParameter,
    TEnumAsByte<EPhysicalSurface>& OutSurfaceType)
{
    OutMetasoundParameter = -1;
    OutSurfaceType = SurfaceType_Default;

    // Perform the line trace
    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::LineTraceSingle(
        WorldContextObject,
        Start,
        End,
        TraceChannel,
        bTraceComplex,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (!bHit || !HitResult.GetComponent())
    {
        return false;
    }

    UPrimitiveComponent* Component = HitResult.GetComponent();
    int32 NumMaterials = Component->GetNumMaterials();

    const bool bUseFallbackMode = (SurfaceData == nullptr);

    // Track if we found Default as a fallback option
    bool bFoundDefault = false;
    int32 DefaultMetasoundParam = -1;

    // Iterate through all materials on the component to find the first valid surface type
    for (int32 i = 0; i < NumMaterials; ++i)
    {
        UMaterialInterface* Material = Component->GetMaterial(i);
        if (!Material)
        {
            continue;
        }

        UPhysicalMaterial* PhysMat = GetPhysicalMaterialFromMaterial(Material);
        if (PhysMat)
        {
            // Extract surface type from physical material
            TEnumAsByte<EPhysicalSurface> SurfaceType = PhysMat->SurfaceType;

            if (bUseFallbackMode)
            {
                // Fallback mode: Prefer non-Default surface types, but accept Default if it's the only option
                if (SurfaceType != SurfaceType_Default)
                {
                    // Found a non-Default surface type, return it with enum value as parameter
                    OutMetasoundParameter = SurfaceType.GetValue();
                    OutSurfaceType = SurfaceType;
                    return true;
                }
                else
                {
                    bFoundDefault = true;
                    DefaultMetasoundParam = 0;  // SurfaceType_Default enum value
                }
            }
            else
            {
                // Curated mode: Check against data asset map
                // Prefer non-Default surface types, but remember Default if it's in the map
                if (SurfaceData->IsValidSurfaceType(SurfaceType))
                {
                    if (SurfaceType != SurfaceType_Default)
                    {
                        // Found a non-Default surface type in the map, return it immediately
                        OutMetasoundParameter = SurfaceData->GetMetasoundParameter(SurfaceType);
                        OutSurfaceType = SurfaceType;
                        return true;
                    }
                    else
                    {
                        // Found Default in the map, remember it as fallback
                        bFoundDefault = true;
                        DefaultMetasoundParam = SurfaceData->GetMetasoundParameter(SurfaceType);
                    }
                }
            }
        }
    }

    // Handle fallback logic - return Default if we found it and nothing else
    if (bFoundDefault)
    {
        OutMetasoundParameter = DefaultMetasoundParam;
        OutSurfaceType = SurfaceType_Default;
        return true;
    }

    // No valid surface type found - return false with OutMetasoundParameter = -1
    // This allows the AnimNotify to handle fallback logic
    return false;
}

UPhysicalMaterial* UDemuteAudioFunctionLibrary::GetPhysicalMaterialFromMaterial(UMaterialInterface* Material)
{
    if (!Material)
    {
        return nullptr;
    }

    // Get the physical material from the material interface
    return Material->GetPhysicalMaterial();
}
