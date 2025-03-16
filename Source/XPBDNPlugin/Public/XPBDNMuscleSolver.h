// XPBDNMuscleSolver.h
#pragma once

#include "CoreMinimal.h"
#include "XPBDNMeshData.h"
#include "XPBDNPlugin.h"
#include "RenderGraphResources.h"

class FXPBDNMuscleSolver
{
public:
    FXPBDNMuscleSolver() : PositionsBuffer(nullptr), ConstraintsBuffer(nullptr), RestDistancesBuffer(nullptr), bUseGPU(false) {}
    void InitializeSolver(const FXPBDNMeshData& Data);
    void SolveConstraints(FRDGBuilder& GraphBuilder, float DeltaTime);
    void SolveDistanceConstraint(int32 ConstraintIdx, float Stiffness, float DeltaTime); // CPU fallback

private:
    void DispatchShader();
    void SetupGPUBuffers(FRDGBuilder& GraphBuilder);
    void DispatchDistanceConstraints(FRDGBuilder& GraphBuilder, float DeltaTime);

    FXPBDNMeshData MeshData;
    FRDGBuffer* PositionsBuffer;
    FRDGBuffer* ConstraintsBuffer;
    FRDGBuffer* RestDistancesBuffer;
    TRefCountPtr<FRDGPooledBuffer> PositionsPooledBuffer;
    bool bUseGPU; // Toggle GPU/CPU based on shader availability
};