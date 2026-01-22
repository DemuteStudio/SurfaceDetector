// Copyright Epic Games, Inc. All Rights Reserved.

#include "DM_SurfaceDetector.h"

#define LOCTEXT_NAMESPACE "FDM_SurfaceDetectorModule"

void FDM_SurfaceDetectorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDM_SurfaceDetectorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDM_SurfaceDetectorModule, DM_SurfaceDetector)