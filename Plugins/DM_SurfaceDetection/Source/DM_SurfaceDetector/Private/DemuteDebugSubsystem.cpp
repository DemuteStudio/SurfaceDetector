#include "DemuteDebugSubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

void UDemuteDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bFootstepLoggingEnabled = false;
    bPrintMaterialEnabled = false;
    ActorDebugSettings.Empty();
}

void UDemuteDebugSubsystem::Deinitialize()
{
    ActorDebugSettings.Empty();
    Super::Deinitialize();
}

void UDemuteDebugSubsystem::SetFootstepLogging(bool toggle)
{
    bFootstepLoggingEnabled = toggle;
    UE_LOG(LogTemp, Warning, TEXT("Global footstep logging %s"), bFootstepLoggingEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

void UDemuteDebugSubsystem::SetPrintMaterial(bool toggle)
{
    bPrintMaterialEnabled = toggle;
    UE_LOG(LogTemp, Warning, TEXT("Global footstep material printing %s"), bPrintMaterialEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

void UDemuteDebugSubsystem::SetDebugMode(int32 mode)
{
    switch (mode)
    {
    case 0:
        // Disable everything
        bPrintMaterialEnabled = false;
        bFootstepLoggingEnabled = false;
        UE_LOG(LogTemp, Warning, TEXT("Footstep debug: All disabled"));
        break;
    case 1:
        // Global printing only
        bPrintMaterialEnabled = true;
        bFootstepLoggingEnabled = false;
        UE_LOG(LogTemp, Warning, TEXT("Footstep debug: Global on-screen printing enabled"));
        break;
    case 2:
        // Global logging only
        bPrintMaterialEnabled = false;
        bFootstepLoggingEnabled = true;
        UE_LOG(LogTemp, Warning, TEXT("Footstep debug: Global logging enabled"));
        break;
    case 3:
        // Both global printing and logging
        bPrintMaterialEnabled = true;
        bFootstepLoggingEnabled = true;
        UE_LOG(LogTemp, Warning, TEXT("Footstep debug: Global printing + logging enabled"));
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid mode. Use 0-3 (0=off, 1=print, 2=log, 3=both)"));
        break;
    }

    if (ActorDebugSettings.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Solo mode active for %d actor(s)"), ActorDebugSettings.Num());
    }
}

void UDemuteDebugSubsystem::AddActorDebugSettings(const FString& ActorName, bool bEnablePrint, bool bEnableLogging)
{
    if (ActorName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add empty actor name"));
        return;
    }

    FActorDebugSettings Settings(bEnablePrint, bEnableLogging);
    ActorDebugSettings.Add(ActorName, Settings);

    UE_LOG(LogTemp, Warning, TEXT("Added '%s' - Print: %s, Logging: %s (Total actors: %d)"),
        *ActorName,
        bEnablePrint ? TEXT("ON") : TEXT("OFF"),
        bEnableLogging ? TEXT("ON") : TEXT("OFF"),
        ActorDebugSettings.Num());
}

void UDemuteDebugSubsystem::RemoveActorDebugSettings(const FString& ActorName)
{
    if (ActorDebugSettings.Remove(ActorName) > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Removed '%s' from per-actor settings (Remaining: %d)"), *ActorName, ActorDebugSettings.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor '%s' was not in per-actor settings"), *ActorName);
    }
}

void UDemuteDebugSubsystem::ClearActorDebugSettings()
{
    int32 PreviousCount = ActorDebugSettings.Num();
    ActorDebugSettings.Empty();
    UE_LOG(LogTemp, Warning, TEXT("Per-actor settings cleared (removed %d actor(s))"), PreviousCount);
}

bool UDemuteDebugSubsystem::ShouldShowPrintForActor(const AActor* Actor) const
{
    if (!Actor)
    {
        return false;
    }

    FString ActorName = Actor->GetName();

    // Check if this actor has solo mode settings
    if (const FActorDebugSettings* Settings = ActorDebugSettings.Find(ActorName))
    {
        return Settings->bEnablePrint;
    }

    // If no solo mode for this actor, use global setting
    return bPrintMaterialEnabled;
}

bool UDemuteDebugSubsystem::ShouldShowLoggingForActor(const AActor* Actor) const
{
    if (!Actor)
    {
        return false;
    }

    FString ActorName = Actor->GetName();

    // Check if this actor has solo mode settings
    if (const FActorDebugSettings* Settings = ActorDebugSettings.Find(ActorName))
    {
        return Settings->bEnableLogging;
    }

    // If no solo mode for this actor, use global setting
    return bFootstepLoggingEnabled;
}

void UDemuteDebugSubsystem::ShouldShow(const AActor* Actor, bool& bOutShouldPrint, bool& bOutShouldLog) const
{
    // Default to false
    bOutShouldPrint = false;
    bOutShouldLog = false;

    if (!Actor)
    {
        return;
    }

    FString ActorName = Actor->GetName();

    // Check if actor has solo mode settings, otherwise use global
    if (const FActorDebugSettings* Settings = ActorDebugSettings.Find(ActorName))
    {
        bOutShouldPrint = Settings->bEnablePrint;
        bOutShouldLog = Settings->bEnableLogging;
    }
    else
    {
        bOutShouldPrint = bPrintMaterialEnabled;
        bOutShouldLog = bFootstepLoggingEnabled;
    }
}

// Helper function to format actor name from BP_ClassName0 to BP_ClassName_C_0
static FString FormatActorName(const FString& InputName)
{
    // Check if the name ends with a digit
    if (InputName.IsEmpty() || !FChar::IsDigit(InputName[InputName.Len() - 1]))
    {
        return InputName; // Return as-is if doesn't end with digit
    }

    // Find where the trailing digits start
    int32 DigitStartIndex = InputName.Len() - 1;
    while (DigitStartIndex > 0 && FChar::IsDigit(InputName[DigitStartIndex - 1]))
    {
        DigitStartIndex--;
    }

    // Check if "_C_" is already present before the digits
    if (DigitStartIndex >= 3)
    {
        FString BeforeDigits = InputName.Left(DigitStartIndex);
        if (BeforeDigits.EndsWith(TEXT("_C_")))
        {
            return InputName; // Already formatted correctly
        }
    }

    // Split the name into prefix and number
    FString Prefix = InputName.Left(DigitStartIndex);
    FString Number = InputName.RightChop(DigitStartIndex);

    // Insert "_C" before the underscore and number
    return FString::Printf(TEXT("%s_C_%s"), *Prefix, *Number);
}

// Console command for footstep debug modes
static FAutoConsoleCommandWithWorldAndArgs FootstepDebugCommand(
    TEXT("DEMUTE.Debug.PrintFootstepMaterial"),
    TEXT("Control footstep debug. Usage:\n  DEMUTE.Debug.PrintFootstepMaterial <0|1|2|3> - Set global mode\n  DEMUTE.Debug.PrintFootstepMaterial <0|1|2|3> <ActorName> -solo - Solo mode for specific actor\nNote: Actor names like BP_ThirdPersonCharacter0 will be auto-formatted to BP_ThirdPersonCharacter_C_0\nExamples:\n  DEMUTE.Debug.PrintFootstepMaterial 1  (enable global printing)\n  DEMUTE.Debug.PrintFootstepMaterial 3 BP_ThirdPersonCharacter0 -solo  (solo mode)"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args, UWorld* World)
        {
            if (Args.Num() < 1)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("Usage:\n  DEMUTE.Debug.PrintFootstepMaterial <0|1|2|3> - Set global mode\n  DEMUTE.Debug.PrintFootstepMaterial <0|1|2|3> <ActorName> -solo - Solo mode for specific actor\n\nGlobal modes:\n  0 = All disabled\n  1 = Global printing only\n  2 = Global logging only\n  3 = Global printing + logging\n\nSolo mode:\n  Disables global debugging and enables only for specific actor\n  Mode number determines what to enable for the actor (1=print, 2=log, 3=both)\n  Actor names like BP_ThirdPersonCharacter0 will be auto-formatted to BP_ThirdPersonCharacter_C_0\n\nExamples:\n  DEMUTE.Debug.PrintFootstepMaterial 2\n  DEMUTE.Debug.PrintFootstepMaterial 3 BP_ThirdPersonCharacter0 -solo\n  DEMUTE.Debug.PrintFootstepMaterial 1 BP_Enemy0 -solo")
                );
                return;
            }

            if (!World)
            {
                UE_LOG(LogTemp, Warning, TEXT("World is null"));
                return;
            }

            UGameInstance* gameInstance = World->GetGameInstance();
            if (!gameInstance)
            {
                UE_LOG(LogTemp, Warning, TEXT("GameInstance is null"));
                return;
            }

            UDemuteDebugSubsystem* DebugSubsystem = gameInstance->GetSubsystem<UDemuteDebugSubsystem>();
            if (!DebugSubsystem)
            {
                UE_LOG(LogTemp, Warning, TEXT("DemuteDebugSubsystem is null"));
                return;
            }

            FString FirstArg = Args[0];

            // Check if first argument is a mode number (0-3)
            if (FirstArg == TEXT("0") || FirstArg == TEXT("1") || FirstArg == TEXT("2") || FirstArg == TEXT("3"))
            {
                int32 mode = FCString::Atoi(*FirstArg);

                // Check if there's a -solo argument
                bool bSoloMode = false;
                FString ActorName;

                if (Args.Num() >= 2)
                {
                    ActorName = Args[1];

                    // Check for -solo flag
                    if (Args.Num() >= 3 && Args[2].ToLower() == TEXT("-solo"))
                    {
                        bSoloMode = true;
                    }
                }

                if (bSoloMode && !ActorName.IsEmpty())
                {
                    // Format the actor name (BP_ClassName0 -> BP_ClassName_C_0)
                    FString FormattedActorName = FormatActorName(ActorName);

                    // Solo mode: disable global, enable for specific actor based on mode
                    DebugSubsystem->SetDebugMode(0); // Disable global

                    bool bEnablePrint = (mode == 1 || mode == 3);
                    bool bEnableLog = (mode == 2 || mode == 3);

                    DebugSubsystem->AddActorDebugSettings(FormattedActorName, bEnablePrint, bEnableLog);

                    UE_LOG(LogTemp, Warning, TEXT("Solo mode: Global disabled, actor '%s' set to mode %d (Print: %s, Log: %s)"),
                        *FormattedActorName, mode,
                        bEnablePrint ? TEXT("ON") : TEXT("OFF"),
                        bEnableLog ? TEXT("ON") : TEXT("OFF"));
                }
                else
                {
                    // Set global debug mode
                    DebugSubsystem->SetDebugMode(mode);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid mode. First argument must be 0-3 (0=off, 1=print, 2=log, 3=both)"));
            }
        }
    )
);

// Console command to clear everything
static FAutoConsoleCommandWithWorldAndArgs ClearFootstepDebuggingCommand(
    TEXT("DEMUTE.Debug.ClearFootstepDebugging"),
    TEXT("Clear all footstep debugging settings (global and solo actors)"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args, UWorld* World)
        {
            if (!World)
            {
                UE_LOG(LogTemp, Warning, TEXT("World is null"));
                return;
            }

            UGameInstance* gameInstance = World->GetGameInstance();
            if (!gameInstance)
            {
                UE_LOG(LogTemp, Warning, TEXT("GameInstance is null"));
                return;
            }

            UDemuteDebugSubsystem* DebugSubsystem = gameInstance->GetSubsystem<UDemuteDebugSubsystem>();
            if (!DebugSubsystem)
            {
                UE_LOG(LogTemp, Warning, TEXT("DemuteDebugSubsystem is null"));
                return;
            }

            // Clear global settings
            DebugSubsystem->SetDebugMode(0);

            // Clear solo actor settings
            DebugSubsystem->ClearActorDebugSettings();

            UE_LOG(LogTemp, Warning, TEXT("All footstep debugging cleared (global + solo actors)"));
        }
    )
);