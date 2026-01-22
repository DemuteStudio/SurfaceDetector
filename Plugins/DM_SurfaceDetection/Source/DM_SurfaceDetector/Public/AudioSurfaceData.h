#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AudioSurfaceData.generated.h"

/**
 * Data Asset that maps Physical Material Surface Types to Metasound parameter values.
 *
 * This asset allows audio designers to define a curated list of which surface types
 * produce sounds, and what Metasound parameter each surface should use. Surfaces not
 * in the map will return -1, allowing fallback handling in AnimNotify Blueprints.
 *
 * Create this asset via Content Browser: Right-click > Miscellaneous > Data Asset > AudioSurfaceData
 */
UCLASS(BlueprintType)
class DM_SURFACEDETECTOR_API UAudioSurfaceData : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Maps Surface Types to Metasound parameter values for sound selection.
     *
     * How to use:
     * - Key: Select surface type from Project Settings (Physics > Physical Surfaces)
     * - Value: Enter the corresponding Metasound parameter (e.g., 0=concrete, 1=wood, 2=metal)
     *
     * Surface Priority:
     * - Non-Default surfaces are ALWAYS preferred over SurfaceType_Default
     * - Default is only used if no other mapped surface type exists on the hit object
     * - If no surfaces in this map are found, returns -1 (handled by AnimNotify fallback)
     *
     * Tip: You can include SurfaceType_Default to define a fallback sound for materials
     * without assigned physical materials, but it will only play if no other mapped
     * surfaces are present on the object.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio Surface")
    TMap<TEnumAsByte<EPhysicalSurface>, int32> SurfaceTypeMap;

    /**
     * Checks if a given surface type exists in the map.
     * @param SurfaceType The surface type to check
     * @return True if the surface type is in the map, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "Audio Surface")
    bool IsValidSurfaceType(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

    /**
     * Gets the Metasound parameter value for a given surface type.
     * @param SurfaceType The surface type to look up
     * @return The Metasound parameter value, or -1 if not found
     */
    UFUNCTION(BlueprintPure, Category = "Audio Surface")
    int32 GetMetasoundParameter(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

#if WITH_EDITOR
    /**
     * Called after a property is changed in the editor.
     */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
