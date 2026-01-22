#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DemuteDebugSubsystem.generated.h"

// Struct to store per-actor debug settings
USTRUCT(BlueprintType)
struct FActorDebugSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Footstep Debug")
    bool bEnablePrint;

    UPROPERTY(BlueprintReadWrite, Category = "Footstep Debug")
    bool bEnableLogging;

    FActorDebugSettings()
        : bEnablePrint(false), bEnableLogging(false)
    {
    }

    FActorDebugSettings(bool InPrint, bool InLogging)
        : bEnablePrint(InPrint), bEnableLogging(InLogging)
    {
    }
};

UCLASS(Blueprintable)
class DM_SURFACEDETECTOR_API UDemuteDebugSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Global logging boolean
    UPROPERTY(BlueprintReadWrite, Category = "Footstep Debug")
    bool bFootstepLoggingEnabled = false;

    // Global print material on-screen boolean
    UPROPERTY(BlueprintReadWrite, Category = "Footstep Debug")
    bool bPrintMaterialEnabled = false;

    // Per-actor debug settings map
    UPROPERTY(BlueprintReadOnly, Category = "Footstep Debug")
    TMap<FString, FActorDebugSettings> ActorDebugSettings;

    // Toggle logging command
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void SetFootstepLogging(bool toggle);

    // Toggle print material command
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void SetPrintMaterial(bool toggle);

    // Set debug mode (0=off, 1=print, 2=log, 3=both)
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void SetDebugMode(int32 mode);

    // Add actor with specific debug settings
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void AddActorDebugSettings(const FString& ActorName, bool bEnablePrint, bool bEnableLogging);

    // Remove actor from settings
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void RemoveActorDebugSettings(const FString& ActorName);

    // Clear all per-actor settings
    UFUNCTION(BlueprintCallable, Category = "Footstep Debug")
    void ClearActorDebugSettings();

    // Check if print should be shown for a specific actor
    UFUNCTION(BlueprintPure, Category = "Footstep Debug")
    bool ShouldShowPrintForActor(const AActor* Actor) const;

    // Check if logging should be shown for a specific actor
    UFUNCTION(BlueprintPure, Category = "Footstep Debug")
    bool ShouldShowLoggingForActor(const AActor* Actor) const;

    // Combined check - returns both print and logging states for an actor
    UFUNCTION(BlueprintPure, Category = "Footstep Debug")
    void ShouldShow(const AActor* Actor, bool& bOutShouldPrint, bool& bOutShouldLog) const;

    // Getter for logging enabled state
    UFUNCTION(BlueprintPure, Category = "Footstep Debug")
    bool IsFootstepLoggingEnabled() const { return bFootstepLoggingEnabled; }

    // Getter for print material enabled state
    UFUNCTION(BlueprintPure, Category = "Footstep Debug")
    bool IsPrintMaterialEnabled() const { return bPrintMaterialEnabled; }

    // Subsystem lifecycle
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};