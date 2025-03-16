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
  - [ ] 1.3. **Set up folder structure**
    - [x] 1.3.1. Create `Source/XPBDNPlugin/Public` and `Private` folders.
    - [ ] 1.3.2. Add `Config` folder with empty `DefaultXPBDNPlugin.ini`.
    - [ ] 1.3.3. Verify structure in VS, rebuild to ensure no path errors.

- [ ] 2. **Implement XPBD solver for muscle simulation**
  - [ ] 2.1. **Define lightweight surface constraints (distance, shape-matching) in `XPBDNMuscleSolver.h/.cpp`**
    - [ ] 2.1.1. Create `XPBDNMuscleSolver.h` with `TArray<FVector> Positions`, `TArray<int32> Constraints`.
    - [ ] 2.1.2. Implement distance constraint solver in `.cpp` (e.g., `SolveDistanceConstraint`).
    - [ ] 2.1.3. Add shape-matching logic (e.g., `ComputeShapeMatchGoal`), test with dummy data.
    - [ ] 2.1.4. Build, log results with `UE_LOG` to verify constraint behavior.
  - [ ] 2.2. **Create HLSL compute shader (`XPBDCompute.hlsl`) for GPU-accelerated constraint solving**
    - [ ] 2.2.1. Define shader with `RWStructuredBuffer<float3>` for positions, `StructuredBuffer<int2>` for constraints.
    - [ ] 2.2.2. Implement XPBD iteration (e.g., `ComputeCorrection`), compile with `FShaderCompiler`.
    - [ ] 2.2.3. Test shader dispatch in `XPBDNMuscleSolver.cpp` using `FRHICommandList`.
  - [ ] 2.3. **Test solver with a simple skeletal mesh (e.g., 10k verts, 50 clusters)**
    - [ ] 2.3.1. Create test mesh (e.g., cube with 10k verts) in Editor.
    - [ ] 2.3.2. Apply solver, check deformation visually and via logs.
    - [ ] 2.3.3. Add nullptr checks (e.g., `if (!Mesh) return;`), rebuild, test edge cases.

- [ ] 3. **Integrate soft body component with Unreal Engine**
  - [ ] 3.1. **Create `UXPBDNComponent` (subclass of `USkeletalMeshComponent`) to manage XPBD and Nanite integration**
    - [ ] 3.1.1. Subclass `USkeletalMeshComponent` in `XPBDNComponent.h`.
    - [ ] 3.1.2. Add `UPROPERTY` for `FXPBDNMuscleSolver* Solver`, initialize in `.cpp`.
    - [ ] 3.1.3. Build, attach to a test actor, verify in Editor.
  - [ ] 3.2. **Add properties for cluster count, stiffness, displacement strength, and morph blending**
    - [ ] 3.2.1. Define `int32 ClusterCount`, `float Stiffness`, `float DisplacementStrength` with `UPROPERTY(EditAnywhere)`.
    - [ ] 3.2.2. Add `float MorphBlendWeight` with `UPROPERTY(EditAnywhere, Meta = (ClampMin = "0.0", ClampMax = "1.0"))` for morph target blending.
    - [ ] 3.2.3. Add default values (e.g., `ClusterCount = 50`, `MorphBlendWeight = 1.0`), rebuild, check in Details panel.
  - [ ] 3.3. **Implement `TickComponent` to dispatch XPBD compute and update Nanite mesh**
    - [ ] 3.3.1. Override `TickComponent`, call `Solver->Solve()` if valid.
    - [ ] 3.3.2. Dispatch `XPBDCompute.hlsl`, update mesh positions with morph blending applied.
    - [ ] 3.3.3. Add nullptr check (`if (!Solver) return;`), test with logging.

- [ ] 4. **Add collision detection and response**
  - [ ] 4.1. **Implement basic vertex-based collision constraints in XPBD (e.g., against bones or static objects)**
    - [ ] 4.1.1. Add `TArray<FVector> CollisionNormals` to `XPBDNMuscleSolver`.
    - [ ] 4.1.2. Implement collision constraint (e.g., push vertices above plane), test with static floor.
    - [ ] 4.1.3. Build, verify collision via debug draw.
  - [ ] 4.2. **Test collision with a simple scene (e.g., sphere vs. floor)**
    - [ ] 4.2.1. Create scene with cube (soft body) and floor (static).
    - [ ] 4.2.2. Run simulation, log collision points, tweak stiffness.
 - [ ] 4.3. **Implement GPU-accelerated XPBD collision clusters using compute shaders**
    - [ ] 4.3.1. Create UAV buffers for XPBD positions and collision clusters using `FRHIBufferCreateDesc`.
    - [ ] 4.3.2. Extend `XPBDCompute.hlsl` with broad-phase (spatial hash) and narrow-phase collision.
    - [ ] 4.3.3. Dispatch via `FRHIComputeCommandList` with `RHIDispatchComputeShader`, target ~1-3 ms/actor.
    - [ ] 4.3.4. Sync with `RHIThreadFence()` before Nanite WPO update.
  - [ ] 4.4. **Create dynamic collision proxy updated with XPBD and morphs**
    - [ ] 4.4.1. Generate a low-poly proxy (1k-5k verts) from `SKM_Quinn.fbx`.
    - [ ] 4.4.2. Update proxy vertices in `TickComponent` with XPBD offsets and `MorphBlendWeight`.
    - [ ] 4.4.3. Integrate with PhysX/Chaos, test against Nanite-rendered mesh.
  - [ ] 4.5. **Implement hybrid XPBD-Chaos Cloth collision for clothing simulation**
    - [ ] 4.5.1. Set up Chaos Cloth for a clothing mesh, driven by Niagara.
    - [ ] 4.5.2. Add GPU collision pass in `XPBDCompute.hlsl` between cloth and XPBD body.
    - [ ] 4.5.3. Test cloth-body interaction, optimize for 60 FPS.
  - [ ] 4.6. **(Experiment) Prototype ray tracing-based collision using Nanite’s dynamic geometries**
    - [ ] 4.6.1. Build `FRayTracingGeometry` from Nanite clusters (needs `NaniteSceneProxy.cpp`).
    - [ ] 4.6.2. Dispatch ray tracing shader via `FRHICommandList`, cast rays from XPBD verts.
    - [ ] 4.6.3. Compare performance (~5 ms?) vs. cluster approach, decide viability.

- [ ] 5. **Optimize simulation performance**
  - [ ] 5.1. **Reduce constraint count via clustering (e.g., 50-100 clusters per actor)**
    - [ ] 5.1.1. Implement clustering algorithm in `XPBDNMuscleSolver` (e.g., k-means on vertices).
    - [ ] 5.1.2. Limit to 50-100 clusters, test deformation quality.
  - [ ] 5.2. **Implement async compute buffers to minimize CPU-GPU sync**
    - [ ] 5.2.1. Use `FRWBuffer` for positions, double-buffer for async updates.
    - [ ] 5.2.2. Integrate with `FRHICommandList`, test sync-free performance.
  - [ ] 5.3. **Profile and tune for ~1-3 ms per actor on RTX 3060**
    - [ ] 5.3.1. Use NVIDIA Nsight to measure compute time (~1-3 ms goal).
    - [ ] 5.3.2. Adjust cluster count and iterations, log results.

- [ ] 6. **Expose simulation parameters to Blueprints**
  - [ ] 6.1. **Add `UPROPERTY` for stiffness, cluster size, and displacement strength in `UXPBDNComponent`**
    - [ ] 6.1.1. Extend `UXPBDNComponent` with Blueprint-readable properties.
    - [ ] 6.1.2. Test visibility in Blueprint editor.
  - [ ] 6.2. **Create Blueprint nodes for enabling/disabling simulation and adjusting parameters**
    - [ ] 6.2.1. Add `UFUNCTION(BlueprintCallable)` for `EnableSimulation`, `SetStiffness`.
    - [ ] 6.2.2. Build, test node functionality in a Blueprint graph.
  - [ ] 6.3. **Test Blueprint integration with a sample actor**
    - [ ] 6.3.1. Create sample actor with `UXPBDNComponent`, adjust params in Blueprint.
    - [ ] 6.3.2. Verify changes affect simulation, log outcomes.

- [ ] 7. **Add basic rendering for soft body mesh**
  - [ ] 7.1. **Enable Nanite support on `SKM_Quinn.fbx` in Skeletal Mesh Editor**
    - [ ] 7.1.1. Import `SKM_Quinn.fbx`, enable Nanite in Skeletal Mesh Editor.
    - [ ] 7.1.2. Test rendering with default material, verify LOD.
  - [ ] 7.2. **Create material with Nanite tessellation and displacement for muscle bulging**
    - [ ] 7.2.1. Create material (`M_XPBDN_Skin`), enable tessellation and displacement.
    - [ ] 7.2.2. Add simple displacement (e.g., `BoneWeight * Strength`), test bulging.
  - [ ] 7.3. **Test rendering performance (~1-3 ms per actor)**
    - [ ] 7.3.1. Profile with `stat rhi` (~1-3 ms goal).
    - [ ] 7.3.2. Adjust tessellation factor, log results.

- [ ] 8. **Support skeletal mesh deformation**
  - [ ] 8.1. Update XPBD positions in GPU buffer, bind as UAV to `SkinNanite.hlsl`.
  - [ ] 8.2. Use `FRHICommandList` to enqueue shader updates, sync with `FinishRecording()`.
  - [ ] 8.3. **Test deformation with animation clips from `SKM_Quinn.fbx`**
    - [ ] 8.3.1. Apply animation from `SKM_Quinn.fbx`, check muscle/skin behavior.
    - [ ] 8.3.2. Add logging for vertex offsets, tweak mapping.

- [ ] 9. **Create unit tests for XPBD solver**
  - [ ] 9.1. **Write test cases for distance constraints (e.g., stretch resistance)**
    - [ ] 9.1.1. Create `TestDistanceConstraint` in `XPBDNTest.h/.cpp`.
    - [ ] 9.1.2. Test stretch resistance, log pass/fail.
  - [ ] 9.2. **Test shape-matching clusters for volume preservation**
    - [ ] 9.2.1. Add `TestShapeMatching`, verify volume preservation.
    - [ ] 9.2.2. Run with dummy cluster, check results.
  - [ ] 9.3. **Automate tests in Unreal’s testing framework**
    - [ ] 9.3.1. Integrate with Unreal’s `AutomationTest` framework.
    - [ ] 9.3.2. Run tests in Editor, ensure all pass.

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

- **File Structure**:
```
 XPBDNPlugin/
  ├── Config/
  │   └── DefaultXPBDNPlugin.ini
  ├── Source/
  │   └── XPBDNPlugin/
  │       ├── Public/
  │       │   ├── XPBDNComponent.h
  │       │   ├── XPBDNMuscleSolver.h
  │       │   ├── XPBDNConstraint.h
  │       │   ├── XPBDNDistanceConstraint.h
  │       │   ├── XPBDNShaderManager.h
  │       │   ├── XPBDNMeshData.h
  │       │   ├── XPBDNBlueprintLibrary.h
  │       │   └── XPBDNTestSuite.h
  │       └── Private/
  │           ├── XPBDNComponent.cpp
  │           ├── XPBDNMuscleSolver.cpp
  │           ├── XPBDNConstraint.cpp
  │           ├── XPBDNDistanceConstraint.cpp
  │           ├── XPBDNShaderManager.cpp
  │           ├── XPBDNBlueprintLibrary.cpp
  │           └── XPBDNTestSuite.cpp
  ├── Shaders/
  │   ├── XPBDCompute.hlsl
  │   └── SkinNanite.hlsl
  └── XPBDNPlugin.uplugin
```

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
