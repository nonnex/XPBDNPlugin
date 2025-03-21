# XPBDN Project Plan (Version 1)

This document outlines the development plan for the XPBDN (Extended Position-Based Dynamics with Nanite) plugin, a new Unreal Engine 5.5.4 plugin designed to simulate realistic muscle and skin behavior for soft body actors using Nanite skeletal mesh support and XPBD. The project leverages GPU acceleration (e.g., RTX 3060) to achieve real-time performance (60 FPS) for 5-10 actors with a total of 225k–450k vertices, integrating animation, morphs, and high-quality skin rendering with substrate shaders.

---

## Goals

- **Real-Time Performance**: Achieve 60 FPS for 5-10 soft body actors, each with 45k+ vertices (225k–450k total), using Nanite’s dynamic LOD and lightweight XPBD simulation on an RTX 3060.
- **Realistic Simulation**: Simulate muscle bulging and skin deformation with Nanite tessellation for visual detail and XPBD for physical behavior, enhanced by substrate shaders.
- **GPU Acceleration**: Utilize HLSL compute shaders for XPBD muscle simulation and Nanite’s GPU-driven rendering pipeline for optimal performance.
- **Streamlined Development**: Build a simple, modular plugin from scratch, leveraging `SKM_Quinn.fbx` as a Nanite skeletal mesh, avoiding complex tetrahedral workflows.

---

## Tasks

- [x] 1. **Setup basic project structure for Unreal Engine 5.5.4 plugin**
  - [x] 1.1. **Create new plugin project named `XPBDNPlugin` in UE 5.5.4**
    - [x] 1.1.1. Launch UE 5.5.4 Editor, select “New Plugin” → “Blank Plugin”.
    - [x] 1.1.2. Name it `XPBDNPlugin`, save to repo folder (e.g., `XPBDNPlugin/Plugins/XPBDNPlugin`).
    - [x] 1.1.3. Build and compile in Visual Studio, verify plugin loads in Editor.
  - [x] 1.2. **Configure `.uplugin` file with dependencies (Core, Engine, RenderCore)**
    - [x] 1.2.1. Edit `XPBDNPlugin.uplugin`, add `"Modules": [{ "Name": "XPBDNPlugin", "Type": "Runtime" }]`.
    - [x] 1.2.2. Add dependencies: `"Dependencies": ["Core", "Engine", "RenderCore"]`.
    - [x] 1.2.3. Rebuild, test Editor recognizes plugin without errors.
  - [x] 1.3. **Set up folder structure**
    - [x] 1.3.1. Create `Source/XPBDNPlugin/Public` and `Private` folders.
    - [x] 1.3.2. Add `Config` folder with `DefaultXPBDNPlugin.ini` (updated with initial settings).
    - [x] 1.3.3. Verify structure in VS, rebuild to ensure no path errors.
  - [x] 1.4. **Implement smart logging system**
    - [x] 1.4.1. Add custom log category `LogXPBDNPlugin` in `XPBDNPlugin.h`.
    - [x] 1.4.2. Define logging macros (`XPBDN_LOG_INFO`, etc.) in `XPBDNPlugin.h`.
    - [x] 1.4.3. Implement log category in `XPBDNPlugin.cpp`, test with initialization log.
  - [x] 1.5. **Enhance configuration support**
    - [x] 1.5.1. Add initial settings to `DefaultXPBDNPlugin.ini` (e.g., `LogVerbosity`).
    - [x] 1.5.2. Read configuration in `XPBDNPlugin.cpp` and log the verbosity setting.
    - [x] 1.5.3. Update `XPBDNPlugin.cpp` to use explicit path to `DefaultXPBDNPlugin.ini` for reliable config loading.
  - [x] 1.6. **Create a simple test scene**
    - [x] 1.6.1. Add placeholder `UXPBDNComponent.h/.cpp` with logging in `BeginPlay`.
    - [x] 1.6.2. Attach to a test actor in the Editor, verify log output.
  - [x] 1.7. **Resolve build issues with `FXPBDNMeshData`**
    - [x] 1.7.1. Separate `FXPBDNMeshData` into `XPBDNMeshData.h` with `XPBDNPLUGIN_API`.
    - [x] 1.7.2. Move `#include "XPBDNMeshData.generated.h"` to top of file for UE 5.5.4 compatibility.
    - [x] 1.7.3. Update `XPBDNMuscleSolver.h/.cpp` to include `XPBDNMeshData.h`, rebuild successfully.

- [ ] 2. **Implement XPBD solver for muscle simulation**
  - [x] 2.1. **Define lightweight surface constraints (distance, shape-matching) in `XPBDNMuscleSolver.h/.cpp`**
    - [x] 2.1.1. Create `XPBDNMuscleSolver.h` with `TArray<FVector> Positions`, `TArray<int32> Constraints` (moved to `XPBDNMeshData.h`).
    - [x] 2.1.2. Implement distance constraint solver in `.cpp` (e.g., `SolveDistanceConstraint`).
    - [x] 2.1.3. Add shape-matching logic (e.g., `ComputeShapeMatchGoal`), test with dummy data.
    - [x] 2.1.4. Build, log results with `UE_LOG` to verify constraint behavior.
  - [ ] 2.2. **Create HLSL compute shader (`XPBDCompute.hlsl`) for GPU-accelerated constraint solving**
    - [x] 2.2.1. Define shader with `RWStructuredBuffer<float3>` for positions (implemented as `XPBDCompute.usf`).
    - [x] 2.2.2. Implement XPBD iteration (e.g., `ComputeCorrection`), compile with `FShaderCompiler`.
      - Status: Implemented in `XPBDCompute.usf` at `Shaders/`; virtual path `/Plugins/XPBDNPlugin/Shaders/`; crash-proof with CPU fallback.
    - [ ] 2.2.3. Test shader dispatch in `XPBDNMuscleSolver.cpp` using RDG and `FRHICommandList`.
      - Status: Path resolved; crash prevented with early file check; runtime validation pending.
  - [ ] 2.3. **Test solver with a simple skeletal mesh (e.g., 10k verts, 50 clusters)**

- [ ] 3. **Buffer Management for XPBD Simulation**
  - [ ] 3.1. **Implement RDG buffer creation and upload**
    - [x] 3.1.1. Create buffers for positions, constraints, and rest distances in `SetupGPUBuffers`
      - Status: Complete, using `FRDGBufferDesc` and `QueueBufferUpload`.
    - [ ] 3.1.2. Validate buffer data integrity across GPU and CPU
      - Status: Pending test run post-shader dispatch validation.
  - [ ] 3.2. **Set up buffer readback and synchronization**
    - [x] 3.2.1. Implement readback with `QueueBufferExtraction` and `FRHIGPUBufferReadback`
      - Status: Implemented, syncs with `BlockUntilGPUIdle`.
    - [ ] 3.2.2. Optimize readback performance
      - Status: Needs async optimization to replace `BlockUntilGPUIdle`.
  - [ ] 3.3. **Ensure seamless buffer integration with compute shaders**
    - [ ] 3.3.1. Validate RDG-to-RHI conversion and shader access
      - Status: In progress, `GetRHI()` conversions working, needs final validation post-build.

- [ ] 4. **Integrate soft body component with Unreal Engine**
  - [ ] 4.1. **Create `UXPBDNComponent` (subclass of `USkeletalMeshComponent`) to manage XPBD and Nanite integration**
    - [ ] 4.1.1. Subclass `USkeletalMeshComponent` in `XPBDNComponent.h`.
    - [ ] 4.1.2. Add `UPROPERTY` for `FXPBDNMuscleSolver* Solver`, initialize in `.cpp`.
    - [ ] 4.1.3. Build, attach to a test actor, verify in Editor.
  - [ ] 4.2. **Add properties for cluster count, stiffness, displacement strength, and morph blending**
    - [ ] 4.2.1. Define `int32 ClusterCount`, `float Stiffness`, `float DisplacementStrength` with `UPROPERTY(EditAnywhere)`.
    - [ ] 4.2.2. Add `float MorphBlendWeight` with `UPROPERTY(EditAnywhere, Meta = (ClampMin = "0.0", ClampMax = "1.0"))` for morph target blending.
    - [ ] 4.2.3. Add default values (e.g., `ClusterCount = 50`, `MorphBlendWeight = 1.0`), rebuild, check in Details panel.
  - [ ] 4.3. **Implement `TickComponent` to dispatch XPBD compute and update Nanite mesh**
    - [ ] 4.3.1. Override `TickComponent`, call `Solver->Solve()` if valid.
    - [ ] 4.3.2. Dispatch `XPBDCompute.hlsl`, update mesh positions with morph blending applied.
    - [ ] 4.3.3. Add nullptr check (`if (!Solver) return;`), test with logging.

- [ ] 5. **Add collision detection and response**
  - [ ] 5.1. **Implement basic vertex-based collision constraints in XPBD (e.g., against bones or static objects)**
    - [ ] 5.1.1. Add `TArray<FVector> CollisionNormals` to `XPBDNMuscleSolver`.
    - [ ] 5.1.2. Implement collision constraint (e.g., push vertices above plane), test with static floor.
    - [ ] 5.1.3. Build, verify collision via debug draw.
  - [ ] 5.2. **Test collision with a simple scene (e.g., sphere vs. floor)**
    - [ ] 5.2.1. Create scene with cube (soft body) and floor (static).
    - [ ] 5.2.2. Run simulation, log collision points, tweak stiffness.
  - [ ] 5.3. **Implement GPU-accelerated XPBD collision clusters using compute shaders**
    - [ ] 5.3.1. Create UAV buffers for XPBD positions and collision clusters using `FRHIBufferCreateDesc`.
    - [ ] 5.3.2. Extend `XPBDCompute.hlsl` with broad-phase (spatial hash) and narrow-phase collision.
    - [ ] 5.3.3. Dispatch via `FRHIComputeCommandList` with `RHIDispatchComputeShader`, target ~1-3 ms/actor.
    - [ ] 5.3.4. Sync with `RHIThreadFence()` before Nanite WPO update.
  - [ ] 5.4. **Create dynamic collision proxy updated with XPBD and morphs**
    - [ ] 5.4.1. Generate a low-poly proxy (1k-5k verts) from `SKM_Quinn.fbx`.
    - [ ] 5.4.2. Update proxy vertices in `TickComponent` with XPBD offsets and `MorphBlendWeight`.
    - [ ] 5.4.3. Integrate with PhysX/Chaos, test against Nanite-rendered mesh.
  - [ ] 5.5. **Implement hybrid XPBD-Chaos Cloth collision for clothing simulation**
    - [ ] 5.5.1. Set up Chaos Cloth for a clothing mesh, driven by Niagara.
    - [ ] 5.5.2. Add GPU collision pass in `XPBDCompute.hlsl` between cloth and XPBD body.
    - [ ] 5.5.3. Test cloth-body interaction, optimize for 60 FPS.
  - [ ] 5.6. **(Experiment) Prototype ray tracing-based collision using Nanite’s dynamic geometries**
    - [ ] 5.6.1. Build `FRayTracingGeometry` from Nanite clusters (needs `NaniteSceneProxy.cpp`).
    - [ ] 5.6.2. Dispatch ray tracing shader via `FRHICommandList`, cast rays from XPBD verts.
    - [ ] 5.6.3. Compare performance (~5 ms?) vs. cluster approach, decide viability.

- [ ] 6. **Optimize simulation performance**
  - [ ] 6.1. **Reduce constraint count via clustering (e.g., 50-100 clusters per actor)**
    - [ ] 6.1.1. Implement clustering algorithm in `XPBDNMuscleSolver` (e.g., k-means on vertices).
    - [ ] 6.1.2. Limit to 50-100 clusters, test deformation quality.
  - [ ] 6.2. **Implement async compute buffers to minimize CPU-GPU sync**
    - [ ] 6.2.1. Use `FRWBuffer` for positions, double-buffer for async updates.
    - [ ] 6.2.2. Integrate with `FRHICommandList`, test sync-free performance.
  - [ ] 6.3. **Profile and tune for ~1-3 ms per actor on RTX 3060**
    - [ ] 6.3.1. Use NVIDIA Nsight to measure compute time (~1-3 ms goal).
    - [ ] 6.3.2. Adjust cluster count and iterations, log results.

- [ ] 7. **Expose simulation parameters to Blueprints**
  - [ ] 7.1. **Add `UPROPERTY` for stiffness, cluster size, and displacement strength in `UXPBDNComponent`**
    - [ ] 7.1.1. Extend `UXPBDNComponent` with Blueprint-readable properties.
    - [ ] 7.1.2. Test visibility in Blueprint editor.
  - [ ] 7.2. **Create Blueprint nodes for enabling/disabling simulation and adjusting parameters**
    - [ ] 7.2.1. Add `UFUNCTION(BlueprintCallable)` for `EnableSimulation`, `SetStiffness`.
    - [ ] 7.2.2. Build, test node functionality in a Blueprint graph.
  - [ ] 7.3. **Test Blueprint integration with a sample actor**
    - [ ] 7.3.1. Create sample actor with `UXPBDNComponent`, adjust params in Blueprint.
    - [ ] 7.3.2. Verify changes affect simulation, log outcomes.

- [ ] 8. **Add basic rendering for soft body mesh**
  - [ ] 8.1. **Enable Nanite support on `SKM_Quinn.fbx` in Skeletal Mesh Editor**
    - [ ] 8.1.1. Import `SKM_Quinn.fbx`, enable Nanite in Skeletal Mesh Editor.
    - [ ] 8.1.2. Test rendering with default material, verify LOD.
  - [ ] 8.2. **Create material with Nanite tessellation and displacement for muscle bulging**
    - [ ] 8.2.1. Create material (`M_XPBDN_Skin`), enable tessellation and displacement.
    - [ ] 8.2.2. Add simple displacement (e.g., `BoneWeight * Strength`), test bulging.
  - [ ] 8.3. **Test rendering performance (~1-3 ms per actor)**
    - [ ] 8.3.1. Profile with `stat rhi` (~1-3 ms goal).
    - [ ] 8.3.2. Adjust tessellation factor, log results.

- [ ] 9. **Support skeletal mesh deformation**
  - [ ] 9.1. Update XPBD positions in GPU buffer, bind as UAV to `SkinNanite.hlsl`.
  - [ ] 9.2. Use `FRHICommandList` to enqueue shader updates, sync with `FinishRecording()`.
  - [ ] 9.3. **Test deformation with animation clips from `SKM_Quinn.fbx`**
    - [ ] 9.3.1. Apply animation from `SKM_Quinn.fbx`, check muscle/skin behavior.
    - [ ] 9.3.2. Add logging for vertex offsets, tweak mapping.

- [ ] 10. **Create unit tests for XPBD solver**
  - [ ] 10.1. **Write test cases for distance constraints (e.g., stretch resistance)**
    - [ ] 10.1.1. Create `TestDistanceConstraint` in `XPBDNTest.h/.cpp`.
    - [ ] 10.1.2. Test stretch resistance, log pass/fail.
  - [ ] 10.2. **Test shape-matching clusters for volume preservation**
    - [ ] 10.2.1. Add `TestShapeMatching`, verify volume preservation.
    - [ ] 10.2.2. Run with dummy cluster, check results.
  - [ ] 10.3. **Automate tests in Unreal’s testing framework**
    - [ ] 10.3.1. Integrate with Unreal’s `AutomationTest` framework.
    - [ ] 10.3.2. Run tests in Editor, ensure all pass.

---

## Architecture

- **Core Components**:
  - `UXPBDNComponent`: Central component managing simulation state, properties, and integration with Nanite skeletal mesh.
  - `FXPBDNMuscleSolver`: Handles XPBD logic, including constraint setup and compute shader dispatch.
  - `FXPBDNConstraint`: Base class for constraints (e.g., distance, collision), with derived classes for specific behaviors.
  - `FXPBDNShaderManager`: Manages shader compilation and dispatch for XPBD compute.
  - `FXPBDNMeshData`: Data structure for mesh positions, velocities, and constraints.
  - `UXPBDNBlueprintLibrary`: Blueprint function library for simulation control.

- **Rendering Pipeline**:
  - Nanite skeletal mesh renders the surface with dynamic LOD and tessellation (~1-3 ms per actor).
  - Custom material with substrate shader enhances skin visuals (~0.5-1 ms).
  - Vertex shader (`SkinNanite.hlsl`) maps XPBD-deformed positions to Nanite mesh vertices.

- **Simulation Pipeline**:
  - XPBD runs on GPU via compute shader (~1-3 ms), using lightweight surface constraints (50-100 clusters per actor).
  - Async buffers minimize CPU-GPU synchronization, ensuring smooth updates.
  - Collision detection integrates with XPBD constraints for basic response.

- **Notes**:
  - In Unreal Engine 5.5.4 (binary installation), the generated header for `USTRUCT()` types (e.g., `XPBDNMeshData.generated.h`) must be included *before* the `USTRUCT()` declaration in the header file (e.g., at the top of `XPBDNMeshData.h`). This deviates from prior conventions where it was typically placed at the bottom. This order ensures UHT processes the reflection data correctly and avoids build errors like `C4430` and `C2039`.

- **File Structure**:
```
 XPBDNPlugin/
  ├── Config/
  │   └── DefaultXPBDNPlugin.ini  (updated with initial settings)
  ├── Source/
  │   └── XPBDNPlugin/
  │       └── Public/
  │           ├── XPBDNPlugin.h  (updated with logging)
  │           ├── XPBDNComponent.h  (new)
  │           ├── XPBDNMuscleSolver.h  (new, placeholder)
  │           ├── XPBDNMeshData.h  (new, with generated include at top)
  │           ├── XPBDNConstraint.h
  │           ├── XPBDNDistanceConstraint.h
  │           ├── XPBDNShaderManager.h
  │           ├── XPBDNBlueprintLibrary.h
  │           └── XPBDNTestSuite.h
  │       └── Private/
  │           ├── XPBDNPlugin.cpp  (updated with logging and config)
  │           ├── XPBDNComponent.cpp  (new)
  │           ├── XPBDNMuscleSolver.cpp  (new, placeholder)
  │           ├── XPBDNConstraint.cpp
  │           ├── XPBDNDistanceConstraint.cpp
  │           ├── XPBDNShaderManager.cpp
  │           ├── XPBDNBlueprintLibrary.cpp
  │           └── XPBDNTestSuite.cpp
  ├── Shaders/
  │   ├── XPBDCompute.hlsl  (new, placeholder)
  │   └── SkinNanite.hlsl
  └── XPBDNPlugin.uplugin
```

---

## Unreal Coding Notes for UE 5.5.4

### RHI Buffer Management
- **Buffer Creation**:
  - Use `RHICreateStructuredBuffer` with `FRHIResourceCreateInfo` instead of raw `FRHIBuffer` or `FRHIBufferCreateDesc`.
  - Flags: `BUF_Dynamic` for frequently updated buffers (e.g., positions), `BUF_Static` for read-only data (e.g., constraints).
  - Example: `PositionsBuffer = RHICreateStructuredBuffer(sizeof(FVector), NumElements, BUF_Dynamic | BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);`
- **Buffer Updates**:
  - Avoid `RHILockBuffer` directly; wrap in `ENQUEUE_RENDER_COMMAND` with `FRHICommandListImmediate::LockBuffer`/`UnlockBuffer`.
  - Example:
    ```cpp
    ENQUEUE_RENDER_COMMAND(UpdateBuffer)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            void* Data = RHICmdList.LockBuffer(Buffer, 0, Size, RLM_WriteOnly);
            FMemory::Memcpy(Data, SourceData, Size);
            RHICmdList.UnlockBuffer(Buffer);
        });
    ```
- **Readback**:
  - Use `FRHIGPUBufferReadback` with `CopyToStagingBuffer` for GPU-to-CPU data transfer.
  - Sync with `FRenderCommandFence`.
  - Example:
    ```cpp
    RHICmdList.CopyToStagingBuffer(Buffer, Readback, 0, Size);
    Fence.BeginFence(); Fence.Wait();
    void* Data = Readback->Lock(Size);
    FMemory::Memcpy(Dest, Data, Size);
    Readback->Unlock();
    ```
- **Dependencies**:
  - `.Build.cs`: Include `"Renderer"` instead of deprecated `"ShaderCore"`.

### Structured Buffers
- **Status**: Not deprecated as of 5.5.4; `RHICreateStructuredBuffer` remains valid (returns `FBufferRHIRef`).
- **Best Practice**: Use `FRDGBuffer` with RDG for dynamic/static structured buffers.
  - Example: `GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector), Num), TEXT("Name"));`

### RDG Compute
- **Shift from RHI**: Use `FRDGBuilder` for compute tasks over raw `ENQUEUE_RENDER_COMMAND`.
- **Execution**: Instantiate via `ENQUEUE_RENDER_COMMAND` on render thread.
  - Example: `ENQUEUE_RENDER_COMMAND(Name)([this](FRHICommandListImmediate& RHICmdList) { FRDGBuilder GraphBuilder(RHICmdList); ...; GraphBuilder.Execute(); });`
- **Buffers**: Create with `FRDGBufferDesc`, upload via `QueueBufferUpload`.
- **Shader Dispatch**: Use `AddPass` with `ERDGPassFlags::Compute`.
  - Convert RDG refs (`FRDGBufferUAVRef`/`FRDGBufferSRVRef`) to RHI refs (`FRHIUnorderedAccessView*`/`FRHIShaderResourceView*`) via `GetRHI()`.
  - Use `TShaderMapRef<T>::GetComputeShader()` to retrieve `FRHIComputeShader*`.
  - **Binary Release Best Practice**:
    - `FComputeShaderUtils::Dispatch`:
      ```cpp
      FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, FIntVector(GroupX, 1, 1));
      ```
    - Builds without `#include "ComputeShaderUtils.h"` if commented out (runtime linkage?), but header often missing in 5.5.4 binary.
- **Binary Release Caveats**:
  - `ComputeShaderUtils.h` unreliable in binary despite `"Renderer"`—use manual binding if needed.
  - Shader files (`.usf`) must be in `Plugins/YourPlugin/` (e.g., `Plugins/XPBDNPlugin/XPBDCompute.usf`) for virtual path `"/Plugins/YourPlugin/XPBDCompute.usf"`.
- **Safeguards**:
  - Check buffers, shaders, and allocations for null.
  - Use one-time logging with `static bool` flags:
    ```cpp
    static bool bLogged = false; if (!bLogged) { XPBDN_LOG_VERBOSE("Detail: %d", Value); bLogged = true; }
    ```
- **TRefCountPtr**: Used for `FRDGPooledBuffer` (e.g., `PositionsPooledBuffer`).
  - **No `Reset()`**: Use `= nullptr` to clear; `IsValid()` to check.
  - Differs from `TSharedPtr`—see `Templates/RefCounting.h`.
- **Module Dependencies**:
  - `"RenderCore"` for RDG; `"Renderer"` may not expose `ComputeShaderUtils.h`.
- **Readback**: Use `QueueBufferExtraction` with `TRefCountPtr<FRDGPooledBuffer>`, then `FRHIGPUBufferReadback::EnqueueCopy`.
- **Shader Robustness**:
  - Missing `.usf` (e.g., `XPBDCompute.usf`) crashes at `ShaderCore.cpp:2528`—no engine fallback.
  - Check with `FPaths::FileExists(FPaths::ProjectPluginsDir() / TEXT("XPBDNPlugin/XPBDCompute.usf"))` at init.
  - Add CPU fallback (e.g., `SolveDistanceConstraint`) to keep plugin functional if GPU shader fails.
- - **Shader File Location**:
  - Current: `Plugins/XPBDNPlugin/Shaders/XPBDCompute.usf`—virtual path must be `/Plugins/XPBDNPlugin/Shaders/XPBDCompute.usf` in `IMPLEMENT_GLOBAL_SHADER`.
  - Alternative: Plugin root (`Plugins/XPBDNPlugin/`)—simpler but less organized.
  - Subfolders require exact virtual path match—case-sensitive on some platforms (e.g., macOS).
- **Fail-Safe**:
  - Missing `.usf` crashes at `ShaderCore.cpp:2528`—use `FPaths::FileExists` at init to toggle GPU/CPU mode.
  - Example: `FString ShaderPath = FPaths::ProjectPluginsDir() / TEXT("XPBDNPlugin/Shaders/XPBDCompute.usf"); bUseGPU = FPaths::FileExists(ShaderPath);`
- - **Buffer Management**:
  - Previous "nasty buffer thing" resolved—`FRDGBuffer` creation and readback with `FRHIGPUBufferReadback` now stable; no nullptr issues remain.
- **Shader Crash Prevention**:
  - Missing `.usf` triggers fatal crash at `ShaderCore.cpp:2528`—no engine default exists.
  - Solution: Check `FPaths::FileExists` before GPU setup; fallback to CPU solver if absent.
  - Example: `bUseGPU = FPaths::FileExists(ShaderPath); if (!bUseGPU) { XPBDN_LOG_ERROR(...); }`
  - No dummy shader option—`IMPLEMENT_GLOBAL_SHADER` requires a real file; pre-check is safest.

### Build Issues
- **Latest Fix**: `TRefCountPtr<FRDGPooledBuffer>::Reset()` is invalid; replaced with assignment to `nullptr`.

### RDG Execution
- **Render Thread**: Instantiate `FRDGBuilder` via `ENQUEUE_RENDER_COMMAND` with `FRHICommandListImmediate`.
  - Example: `ENQUEUE_RENDER_COMMAND(Name)([this, DeltaTime](FRHICommandListImmediate& RHICmdList) { FRDGBuilder GraphBuilder(RHICmdList); ...; GraphBuilder.Execute(); });`
- **Readback**: Use `QueueBufferExtraction` with `TRefCountPtr<FRDGPooledBuffer>`, then `FRHIGPUBufferReadback::EnqueueCopy` with offset and size.
- **Niagara Note**: RDG passes for compute, extraction for readback—no direct `DrawIndirect` needed for XPBD.

---

## Milestones

1. **Initial Setup**: Project structure, Nanite-enabled mesh rendering (~1 week).
 - Complete tasks 1.1–1.3 and 7.1 for a functional base.
2. **XPBD Prototype**: Working XPBD solver with basic constraints (~2 weeks).
 - Complete tasks 2.1–2.3 for muscle simulation.
3. **Component Integration**: `UXPBDNComponent` with Nanite and Blueprint support (~2 weeks).
 - Complete tasks 3.1–3.3 and 6.1–6.3.
4. **Rendering and Deformation**: Full Nanite tessellation and skeletal deformation (~2 weeks).
 - Complete tasks 7.2–7.3 and 8.1–8.3.
5. **Optimization and Polish**: Collision, performance tuning, and unit tests (~3 weeks).
 - Complete tasks 4.1–4.3, 5.1–5.3, and 9.1–9.3.

**Total Estimated Time**: ~10 weeks.

---

## Dependencies

- **Unreal Engine 5.5.4**: Required for experimental Nanite skeletal mesh support and tessellation.
- **RTX 3060 (or equivalent)**: Target GPU for development, testing, and performance profiling.
- **HLSL Shader Tools**: Unreal’s shader compiler and RHI for compute and vertex shaders.
- **Sample Asset**: `SKM_Quinn.fbx` (45k+ verts, skeletal rigging, animations, morphs) as primary test mesh.

---

## Risks

- **Nanite Experimental Status**: Bugs in 5.5.4’s skeletal mesh support (e.g., tessellation glitches) may require workarounds or a delay until 5.6.
- **Performance Bottlenecks**: XPBD constraint solving could exceed ~1-3 ms per actor—mitigate with clustering and profiling.
- **Displacement Realism**: Nanite tessellation alone may lack physical muscle behavior—XPBD ensures dynamic simulation.
- **Cloth Integration**: Handling clothes (e.g., separate meshes) may require additional simulation layers—mitigate with Unreal’s cloth system or custom XPBD.

---

## Resources

- **Team**: Solo developer or small team with Unreal C++, HLSL, and Nanite experience.
- **Documentation**: Unreal Engine 5.5.4 Nanite skeletal mesh docs, XPBD research papers (e.g., Müller et al., 2017).
- **Community**: X posts (e.g., @DylserX, April 2024), Unreal forums for Nanite troubleshooting and tips.
- **Tools**: Visual Studio 2022, Unreal Editor 5.5.4, NVIDIA Nsight for GPU profiling, GitHub for version control.

---