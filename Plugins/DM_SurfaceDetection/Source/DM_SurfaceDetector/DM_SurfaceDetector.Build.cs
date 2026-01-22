// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DM_SurfaceDetector : ModuleRules
{
	public DM_SurfaceDetector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"PhysicsCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Add private dependencies here if needed
			}
		);
	}
}
