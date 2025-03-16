#pragma once

#include "CoreMinimal.h"
#include "XPBDNShaderManager.h"
#include "XPBDNMeshData.h"
#include "RenderGraphResources.h"
#include "XPBDNPlugin.h"

class FXPBDNMuscleSolver
{
public:
    FXPBDNMuscleSolver();
    ~FXPBDNMuscleSolver();

    void Initialize();
    void SetupGPUBuffers(FRHICommandListImmediate& RHICmdList);
    void Solve(FRHICommandListImmediate& RHICmdList, float DeltaTime);

private:
    void SolveDistanceConstraint(); // CPU fallback

    TArray<FVector> Positions;
    TArray<FIntPoint> Constraints;
    TArray<float> RestLengths;

    TRefCountPtr<FRDGPooledBuffer> PositionsBuffer;
    TRefCountPtr<FRDGPooledBuffer> ConstraintsBuffer;
    TRefCountPtr<FRDGPooledBuffer> RestLengthsBuffer;

    FXPBDNShaderManager ShaderManager;
    bool bUseGPU;
};