#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AudioSurfaceData.h"
#include "DemuteAudioFunctionLibrary.generated.h"

/**
 * Blueprint Function Library for audio-related utility functions.
 * Provides surface detection functionality for dynamic footstep and foley audio systems.
 * Extracts physical material surface types from line trace hits and maps them to Metasound parameters.
 */
UCLASS()
class DM_SURFACEDETECTOR_API UDemuteAudioFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Performs a line trace and returns the Metasound parameter for the first valid surface type found.
     *
     * IMPORTANT: Both modes prefer non-Default surface types, returning Default only as a last resort.
     *
     * Behavior:
     * - If SurfaceData is provided (Curated Mode):
     *   - Uses the curated list from the data asset (checks SurfaceTypeMap)
     *   - Prefers non-Default surface types in the map
     *   - Returns Default (with its mapped value) ONLY if no other mapped surface type is found
     *   - Returns false with -1 if no mapped surface types found at all
     *
     * - If SurfaceData is null (Fallback Mode):
     *   - Uses surface types from Project Settings (Physics > Physical Surfaces)
     *   - Prefers non-Default surface types (returns first non-Default found)
     *   - Returns Default (value 0) ONLY if no other surface type exists
     *   - Returns false with -1 if trace misses or hit object has no physical materials
     *
     * Return Values:
     * - OutMetasoundParameter with SurfaceData: The mapped value from the data asset, or -1 if no match
     * - OutMetasoundParameter without SurfaceData: The EPhysicalSurface enum value (0=Default, 1-62), or -1 if trace miss
     * - OutSurfaceType: The selected surface type, or SurfaceType_Default if function returns false
     *
     * @param WorldContextObject World context for the trace
     * @param Start Start location of the trace
     * @param End End location of the trace
     * @param TraceChannel The trace channel to use
     * @param bTraceComplex Whether to trace against complex collision
     * @param ActorsToIgnore Array of actors to ignore during the trace
     * @param SurfaceData Optional data asset containing the map of valid surface types (leave null to use Project Settings)
     * @param OutMetasoundParameter The Metasound parameter value for the surface (-1 if no valid surface found)
     * @param OutSurfaceType The surface type that was selected (SurfaceType_Default if none found)
     * @return True if a valid surface type was found, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|Physical Material", meta = (WorldContext = "WorldContextObject"))
    static bool LineTraceForSurfaceTypes(
        UObject* WorldContextObject,
        const FVector& Start,
        const FVector& End,
        ETraceTypeQuery TraceChannel,
        bool bTraceComplex,
        const TArray<AActor*>& ActorsToIgnore,
        UAudioSurfaceData* SurfaceData,
        int32& OutMetasoundParameter,
        TEnumAsByte<EPhysicalSurface>& OutSurfaceType
    );

private:
    /**
     * Extracts the physical material from a UMaterialInterface.
     * @param Material The material interface to extract from
     * @return Physical material if found, or nullptr
     */
    static UPhysicalMaterial* GetPhysicalMaterialFromMaterial(UMaterialInterface* Material);
};
