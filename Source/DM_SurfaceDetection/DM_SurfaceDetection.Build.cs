// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DM_SurfaceDetection : ModuleRules
{
	public DM_SurfaceDetection(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"DM_SurfaceDetection",
			"DM_SurfaceDetection/Variant_Platforming",
			"DM_SurfaceDetection/Variant_Platforming/Animation",
			"DM_SurfaceDetection/Variant_Combat",
			"DM_SurfaceDetection/Variant_Combat/AI",
			"DM_SurfaceDetection/Variant_Combat/Animation",
			"DM_SurfaceDetection/Variant_Combat/Gameplay",
			"DM_SurfaceDetection/Variant_Combat/Interfaces",
			"DM_SurfaceDetection/Variant_Combat/UI",
			"DM_SurfaceDetection/Variant_SideScrolling",
			"DM_SurfaceDetection/Variant_SideScrolling/AI",
			"DM_SurfaceDetection/Variant_SideScrolling/Gameplay",
			"DM_SurfaceDetection/Variant_SideScrolling/Interfaces",
			"DM_SurfaceDetection/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
