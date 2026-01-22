# DM Surface Detection Plugin

**Version:** 1.0.0
**Engine Compatibility:** Unreal Engine 5.7+
**Created by:** Demute Studio
**License:** GNU GPL v3.0

## Overview

DM Surface Detection is a runtime plugin for Unreal Engine that provides surface-aware audio systems. It enables dynamic footstep and movement audio that responds to physical materials in your level, integrating seamlessly with MetaSounds for advanced audio playback.

## Features

- **Runtime Surface Detection** - Line trace utilities that identify physical materials
- **Blueprint Function Library** - Easy-to-use Blueprint nodes for surface queries
- **Data Asset Workflow** - Designer-friendly mapping of physical materials to audio parameters
- **MetaSound Integration** - Included MetaSound container with 16-way switching and pitch/volume modulation
- **AnimNotify Blueprint** - Ready-to-use AnimNotify for footstep audio in animation sequences
- **Debug Subsystem** - Game instance subsystem for per-actor debug logging

## Installation

### Method 1: Plugin Folder (Recommended)

1. Copy the entire `DM_SurfaceDetection` folder to your project's `Plugins/` directory
2. Open your `.uproject` file
3. When prompted to rebuild the plugin, click **Yes**
4. Wait for compilation to complete (may take a few minutes on first build)

### Method 2: Manual Compilation

1. Copy the plugin to `Plugins/DM_SurfaceDetection`
2. Right-click your `.uproject` file → **Generate Visual Studio project files**
3. Open the generated `.sln` file in Visual Studio
4. Build Solution (Ctrl+Shift+B)
5. Launch Unreal Editor

### Verification

1. Open Unreal Editor
2. Go to **Edit → Plugins**
3. Search for "DM Surface Detection"
4. Ensure the plugin is enabled (checkbox checked)
5. Restart the editor if prompted

### ⚠️ Known Issues: Blueprint Reference Fixing Required

When copying Blueprint assets between plugins, internal path references may break. You'll need to fix these once after installation.

#### MetaSound References

**If you see "class not found" errors** when opening the MetaSound:

The `MSS_Switch_16_Mono_PitchAndVolumeModulation` MetaSound references base assets in `Content/Metasounds/Base/`. When first copied to a new project, these references may still point to the original plugin location.

**To fix:**
1. Enable plugin content: **Content Browser Settings** → **Show Plugin Content** ✓
2. Navigate to `Plugins/DM Surface Detection Content/Metasounds/`
3. Open `MSS_Switch_16_Mono_PitchAndVolumeModulation` in the MetaSound Editor
4. Look for red error nodes
5. Delete broken reference nodes and re-add them from the `/Base/` folder
6. Save the MetaSound

**See `FIXING_REFERENCES.md` for detailed step-by-step instructions.**

#### AnimNotify Crash Issue

**If the editor crashes** when adding `AN_Footstep_Surface_MetaSoundParam` to an animation:

The AnimNotify Blueprint has broken internal references that cause access violations. **You'll need to recreate it from scratch.**

**Quick fix:**
1. See `ANIMNOTIFY_ISSUE.md` for complete instructions
2. Create a new AnimNotify Blueprint
3. Implement the logic using `LineTraceForSurfaceTypes` function
4. Takes ~5 minutes

**Alternative:** Ask me to create a C++ AnimNotify class for you (cleaner and more portable).

After fixing these references once, they will persist correctly. These are one-time setup issues when first installing the plugin.

## Quick Start Guide

### Step 1: Configure Physical Materials

1. Open **Project Settings → Physics → Physical Surface**
2. Add surface types for your game (e.g., `Wood`, `Metal`, `Grass`, `Stone`)
3. Create Physical Material assets in Content Browser:
   - Right-click → **Physics → Physical Material**
   - Name them descriptively (e.g., `PM_Wood`, `PM_Metal`)
   - Set the **Surface Type** property to match your project settings

### Step 2: Apply Physical Materials to Level

Assign physical materials to your level geometry:
- **Static Meshes:** Set `Phys Material Override` on Static Mesh Component
- **Landscapes:** Use Landscape Layer physical materials
- **BSP:** Set physical material in Material properties

### Step 3: Create Audio Surface Data Asset

1. In Content Browser, right-click → **Miscellaneous → Data Asset**
2. Select **AudioSurfaceData** as the class
3. Name it (e.g., `DA_FootstepSurfaces`)
4. Open the asset and populate the **Surface To Parameter Map**:
   - **Key:** Physical Material (e.g., `PM_Wood`)
   - **Value:** Integer parameter (0-15 for use with included MetaSound)
5. Set a **Fallback Physical Material** for unmapped surfaces

**Example Mapping:**
```
PM_Wood       → 0
PM_Metal      → 1
PM_Grass      → 2
PM_Stone      → 3
PM_Concrete   → 4
...
```

See the included `Content/Examples/UAudioDataAsset_ValidSurfaces` for reference.

### Step 4: Set Up MetaSound Audio

1. Navigate to `Plugins/DM Surface Detection Content/Metasounds/`
2. Open `MSS_Switch_16_Mono_PitchAndVolumeModulation`
3. Create a new MetaSound Source based on this or use it directly
4. In the MetaSound graph, connect your audio waves to the 16 input slots (0-15)
5. Each slot corresponds to the parameter values you set in your AudioSurfaceData asset

**Important:** The included MetaSound supports 16 different surface types (parameters 0-15). If you need more, duplicate and modify the MetaSound graph.

### Step 5: Add AnimNotify to Animation

1. Open your character's locomotion animation (walk/run cycle)
2. Find frames where feet contact the ground
3. Right-click the Notify track → **Add Notify** → **AN_Footstep_Surface_MetaSoundParam**
4. In the AnimNotify properties:
   - **Audio Surface Data:** Select your DA_FootstepSurfaces asset
   - **Metasound:** Select your configured MetaSound
   - **Trace Channel:** Set to `Visibility` or custom trace channel
   - **Trace Length:** Distance to trace below character (e.g., 100cm)
   - **Socket Name:** Foot bone/socket name (e.g., `foot_l`, `foot_r`)

The AnimNotify will automatically:
- Trace downward from the foot bone
- Detect the physical material
- Look up the parameter in your AudioSurfaceData
- Trigger the MetaSound with the correct surface parameter

## Blueprint Usage

### LineTraceForSurfaceTypes Function

Core function available in Blueprint via **Demute Audio Function Library**.

**Inputs:**
- **World Context Object:** Self or GetWorld()
- **Start:** Start location of line trace
- **End:** End location of line trace
- **Trace Channel:** ETraceTypeQuery (e.g., Visibility)
- **Trace Complex:** Whether to trace against complex collision
- **Actors To Ignore:** Array of actors to exclude from trace
- **Surface Data:** AudioSurfaceData asset (optional)

**Outputs:**
- **Return Value:** `true` if trace hit something, `false` if miss
- **Out Metasound Parameter:** Integer parameter for MetaSound (0-15, or -1 if not found)
- **Out Surface Type:** Physical surface enum value

**Example Blueprint:**
```
Event Tick
  → Get Actor Location (Self)
  → Make Vector (X, Y, Z - 100) [Trace downward 100 units]
  → Line Trace For Surface Types
      - Start: Actor Location
      - End: Location - 100Z
      - Trace Channel: Visibility
      - Surface Data: DA_FootstepSurfaces
  → Branch (if hit)
      → Play MetaSound [Use Out Metasound Parameter]
```

### Two Operating Modes

**Curated Mode (With AudioSurfaceData):**
- Only surfaces in your data asset return valid parameters
- Gives you explicit control over which surfaces produce audio
- Returns `-1` for surfaces not in the map
- **Best for production**

**Fallback Mode (Without AudioSurfaceData):**
- Accepts any surface type from project settings
- Automatically skips `SurfaceType_Default`
- Returns physical surface enum value (0-62) as parameter
- Returns `-1` only on trace miss
- **Best for prototyping**

## Debug System

### DemuteDebugSubsystem

Game Instance Subsystem for debug logging. Access in Blueprint via **Get Game Instance → Get Demute Debug Subsystem**.

**Properties:**
- **Footstep Logging Enabled:** Global toggle for footstep debug logs
- **Actor Debug Settings:** Per-actor debug configuration (TMap)

**Usage:**
Enable debug output to see surface detection results in Output Log.

## Content Included

```
DM_SurfaceDetection/
├── Content/
│   ├── AnimNotifies/
│   │   └── AN_Footstep_Surface_MetaSoundParam.uasset
│   ├── Metasounds/
│   │   ├── MSS_Switch_16_Mono_PitchAndVolumeModulation.uasset [Main asset]
│   │   └── Base/
│   │       ├── MSS_Switch_16_Mono.uasset                     [Switch container]
│   │       ├── DEM_Switch_16_Mono_MSS.uasset                 [Switch source graph]
│   │       └── DEM_GetPitchShift_MSP.uasset                  [Pitch modulation patch]
│   └── Examples/
│       └── UAudioDataAsset_ValidSurfaces.uasset
├── Source/
│   └── DM_SurfaceDetector/ [Runtime module]
└── DM_SurfaceDetection.uplugin
```

## Architecture

### Module: DM_SurfaceDetector (Runtime)

**Dependencies:** Core, CoreUObject, Engine, PhysicsCore

**Key Classes:**

**UAudioSurfaceData** - `AudioSurfaceData.h`
- Data asset mapping physical materials to MetaSound parameters
- TMap<UPhysicalMaterial*, int32> for surface mappings
- Fallback material for unmapped surfaces

**UDemuteAudioFunctionLibrary** - `DemuteAudioFunctionLibrary.h`
- Blueprint Function Library
- `LineTraceForSurfaceTypes()` - Main surface detection function
- Static utility functions for surface queries

**UDemuteDebugSubsystem** - `DemuteDebugSubsystem.h`
- Game Instance Subsystem
- Per-actor debug settings
- Global debug toggles

## Workflow Tips

### Best Practices

1. **Physical Material Naming:** Use prefixes like `PM_` for clarity
2. **Data Asset Organization:** Create one AudioSurfaceData per character type if needed
3. **MetaSound Slots:** Keep parameter mappings documented in your AudioSurfaceData asset names
4. **Trace Channel:** Consider creating a custom trace channel for footsteps to avoid hitting unwanted objects
5. **Trace Length:** Adjust based on character height and terrain variation

### Performance Considerations

- Line traces are inexpensive but add up; use sparingly in Tick events
- AnimNotifies trigger only on specific frames, making them more efficient
- Consider pooling MetaSound instances for many simultaneous characters

### Common Issues

**Trace always returns -1:**
- Check that your level geometry has physical materials assigned
- Verify the trace channel can hit your floor (collision settings)
- Ensure the trace length reaches the ground

**Wrong sounds playing:**
- Verify parameter mappings in AudioSurfaceData match MetaSound slot numbers
- Check that physical materials on level match those in your data asset

**Sounds not playing at all:**
- Ensure MetaSound has audio waves assigned to slots
- Check that the MetaSound parameter is being received correctly
- Verify AnimNotify socket name matches your skeletal mesh bone names

## Extending the Plugin

### Adding More Surface Types

1. Add surface types in **Project Settings → Physics**
2. Create corresponding Physical Materials
3. Add mappings in your AudioSurfaceData asset
4. If you need more than 16 types, duplicate the MetaSound and add more switch inputs

### Custom AnimNotifies

You can create custom AnimNotifies that use `UDemuteAudioFunctionLibrary::LineTraceForSurfaceTypes()` for different use cases:
- Foley sounds (hand touches, body impacts)
- Vehicle wheel sounds
- Weapon dragging sounds

## Support

- **GitHub:** https://github.com/DemuteStudio/UnrealAudioTools
- **Website:** https://www.demute.studio/
- **Issues:** Report bugs via GitHub Issues

## License

This plugin is licensed under **GNU GPL v3.0**. See LICENSE file for details.

## Credits

**Original Development:** Demute Studio - UnrealAudioTools
**Plugin Packaging:** Surface Detection Module Extraction v1.0.0

---

**Version History:**

**1.0.0** (2026-01-20)
- Initial standalone release
- Extracted from UnrealAudioTools multi-module plugin
- Runtime-only module for maximum portability
- Includes example MetaSound and AnimNotify
