# XPBDN Project Plan (Version 1)

This document outlines the development plan for the XPBDN (Extended Position-Based Dynamics with Nanite) plugin, 
a new Unreal Engine 5.5.4 plugin designed to simulate realistic muscle and skin behavior for soft body actors using Nanite skeletal mesh support and XPBD. 
The project leverages GPU acceleration (e.g., RTX 3060) to achieve real-time performance (60 FPS) for 5-10 actors with a total of 225k–450k vertices, 
integrating animation, morphs, and high-quality skin rendering with substrate shaders.

---

## Goals

- **Real-Time Performance**: Achieve 60 FPS for 5-10 soft body actors, each with 45k+ vertices (225k–450k total), using Nanite and XPBD on an RTX 3060.
- **Realistic Simulation**: Simulate muscle bulging, contraction, and skin behavior with dynamic LOD and tessellation via Nanite, enhanced by substrate shaders.
- **GPU Acceleration**: Leverage HLSL compute shaders for XPBD muscle simulation and Nanite’s GPU-driven rendering pipeline.
- **Simplified Workflow**: Use Nanite skeletal meshes directly (e.g., `SKM_Quinn.fbx`), avoiding tetrahedral mesh generation for a streamlined approach.

---

## Tasks

- [ ] Setup basic project structure for Unreal Engine 5.5.4 plugin
  - [ ] Create new plugin project named `XPBDNPlugin` in UE 5.5.4.
  - [ ] Configure `.uplugin` file with dependencies (Core, Engine, RenderCore, Nanite).
  - [ ] Set up folder structure: `Source/XPBDNPlugin/Public`, `Source/XPBDNPlugin/Private`, `Config`.

- [ ] Implement XPBD solver for muscle simulation
  - [ ] Define lightweight surface constraints (distance, shape-matching) in `XPBDNMuscleSolver.h/.cpp`.
  - [ ] Create HLSL compute shader (`XPBDCompute.hlsl`) for GPU-accelerated constraint solving.
  - [ ] Test solver with a simple skeletal mesh (e.g., 10k verts, 50 clusters).

- [ ] Integrate soft body component with Unreal Engine
  - [ ] Create `UXPBDNComponent` (subclass of `USkeletalMeshComponent`) to manage XPBD and Nanite integration.
  - [ ] Add properties for cluster count, stiffness, and displacement strength.
  - [ ] Implement `TickComponent` to dispatch XPBD compute and update Nanite mesh.

- [ ] Add collision detection and response
  - [ ] Implement basic vertex-based collision constraints in XPBD (e.g., against bones or static objects).
  - [ ] Test collision with a simple scene (e.g., sphere vs. floor).
  - [ ] Explore GPU-accelerated collision queries using Nanite data (future optimization).

- [ ] Optimize simulation performance
  - [ ] Reduce constraint count via clustering (e.g., 50-100 clusters per actor).
  - [ ] Implement async compute buffers to minimize CPU-GPU sync.
  - [ ] Profile and tune for ~1-3 ms per actor on RTX 3060.

- [ ] Expose simulation parameters to Blueprints
  - [ ] Add `UPROPERTY` for stiffness, cluster size, and displacement strength in `UXPBDNComponent`.
  - [ ] Create Blueprint nodes for enabling/disabling simulation and adjusting parameters.
  - [ ] Test Blueprint integration with a sample actor.

- [ ] Add basic rendering for soft body mesh
  - [ ] Enable Nanite support on `SKM_Quinn.fbx` in Skeletal Mesh Editor.
  - [ ] Create material with Nanite tessellation and displacement for muscle bulging.
  - [ ] Test rendering performance (~1-3 ms per actor).

- [ ] Support skeletal mesh deformation
  - [ ] Integrate Nanite skeletal animation with XPBD-deformed positions.
  - [ ] Add vertex mapping from XPBD results to Nanite mesh in `SkinNanite.hlsl`.
  - [ ] Test deformation with animation clips from `SKM_Quinn.fbx`.

- [ ] Create unit tests for XPBD solver
  - [ ] Write test cases for distance constraints (e.g., stretch resistance).
  - [ ] Test shape-matching clusters for volume preservation.
  - [ ] Automate tests in Unreal’s testing framework.

---

## Architecture

- **Core Components**:
  - `UXPBDNComponent`: Manages XPBD simulation and Nanite integration, attached to skeletal mesh actors.
  - `FXPBDNMuscleSolver`: CPU-side logic for setting up XPBD constraints and dispatching compute shaders.
  - `XPBDCompute.hlsl`: GPU compute shader for solving XPBD constraints (distance, shape-matching).

- **Rendering Pipeline**:
  - Nanite skeletal mesh handles base rendering and tessellation (~1-3 ms per actor).
  - Custom material with substrate shader for skin visuals (~0.5-1 ms).
  - Vertex shader (`SkinNanite.hlsl`) maps XPBD positions to Nanite mesh.

- **Simulation Pipeline**:
  - XPBD runs on GPU (~1-3 ms), updating vertex positions via shape-matching clusters.
  - Async buffers ensure smooth CPU-GPU data flow.

---

## Milestones

1. **Initial Setup**: Basic plugin structure and Nanite skeletal mesh rendering (~1 week).
2. **XPBD Prototype**: Functional XPBD solver with simple constraints (~2 weeks).
3. **Integration**: `UXPBDNComponent` with Nanite and Blueprint support (~2 weeks).
4. **Optimization**: Collision, performance tuning, and full test scene (~3 weeks).
5. **Polish**: Unit tests, documentation, and final profiling (~2 weeks).

**Total Estimated Time**: ~10 weeks.

---

## Dependencies

- **Unreal Engine 5.5.4**: Required for Nanite skeletal mesh support (experimental).
- **RTX 3060 (or equivalent)**: Target GPU for testing and optimization.
- **HLSL Shader Tools**: Unreal’s shader compiler for compute and vertex shaders.
- **Sample Asset**: `SKM_Quinn.fbx` (45k+ verts, skeletal rigging) as test mesh.

---

## Risks

- **Nanite Stability**: Experimental skeletal mesh support in 5.5.4 may have bugs (e.g., tessellation glitches), requiring workarounds or a 5.6 update.
- **Performance Overruns**: XPBD constraint count could push beyond 16.67 ms budget—mitigate with clustering and profiling.
- **Displacement Limits**: Nanite tessellation may not fully replicate muscle physics—XPBD ensures robustness.
- **Learning Curve**: Adapting to Nanite’s pipeline may slow initial development—mitigate with Epic’s docs and community resources.

---

## Resources

- **Team**: Solo developer or small team with Unreal C++/HLSL expertise.
- **Documentation**: Unreal Engine 5.5.4 Nanite docs, XPBD papers (e.g., Müller et al.).
- **Community**: X posts (e.g., @DylserX), Unreal forums for Nanite updates.
- **Tools**: Visual Studio, Unreal Editor, NVIDIA Nsight for profiling.

---