#include "XPBDNMuscleSolver.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

FXPBDNMuscleSolver::FXPBDNMuscleSolver()
    : bUseGPU(false)
{
}

FXPBDNMuscleSolver::~FXPBDNMuscleSolver()
{
}

void FXPBDNMuscleSolver::Initialize()
{
    bUseGPU = ShaderManager.Initialize();
    if (!bUseGPU)
    {
        XPBDN_LOG_WARNING("Falling back to CPU solver due to GPU shader issues.");
    }
}

void FXPBDNMuscleSolver::SetupGPUBuffers(FRHICommandListImmediate& RHICmdList)
{
    if (!bUseGPU || Positions.Num() == 0 || Constraints.Num() == 0)
    {
        static bool bLoggedSetupFail = false;
        if (!bLoggedSetupFail)
        {
            XPBDN_LOG_WARNING("Skipping GPU buffer setup: GPU disabled or no data.");
            bLoggedSetupFail = true;
        }
        return;
    }

    FRDGBuilder GraphBuilder(RHICmdList);

    FRDGBufferDesc PositionsDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector), Positions.Num());
    FRDGBuffer* PositionsRDG = GraphBuilder.CreateBuffer(PositionsDesc, TEXT("XPBDNPositions"));
    GraphBuilder.QueueBufferUpload(PositionsRDG, Positions.GetData(), Positions.Num() * sizeof(FVector));

    FRDGBufferDesc ConstraintsDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FIntPoint), Constraints.Num());
    FRDGBuffer* ConstraintsRDG = GraphBuilder.CreateBuffer(ConstraintsDesc, TEXT("XPBDNConstraints"));
    GraphBuilder.QueueBufferUpload(ConstraintsRDG, Constraints.GetData(), Constraints.Num() * sizeof(FIntPoint));

    FRDGBufferDesc RestLengthsDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(float), RestLengths.Num());
    FRDGBuffer* RestLengthsRDG = GraphBuilder.CreateBuffer(RestLengthsDesc, TEXT("XPBDNRestLengths"));
    GraphBuilder.QueueBufferUpload(RestLengthsRDG, RestLengths.GetData(), RestLengths.Num() * sizeof(float));

    PositionsBuffer = GraphBuilder.ConvertToExternalBuffer(PositionsRDG);
    ConstraintsBuffer = GraphBuilder.ConvertToExternalBuffer(ConstraintsRDG);
    RestLengthsBuffer = GraphBuilder.ConvertToExternalBuffer(RestLengthsRDG);

    GraphBuilder.Execute();

    static bool bLoggedSetup = false;
    if (!bLoggedSetup)
    {
        XPBDN_LOG_INFO("GPU buffers set up: %d positions, %d constraints.", Positions.Num(), Constraints.Num());
        bLoggedSetup = true;
    }
}

void FXPBDNMuscleSolver::Solve(FRHICommandListImmediate& RHICmdList, float DeltaTime)
{
    if (!bUseGPU || !PositionsBuffer || !ConstraintsBuffer || !RestLengthsBuffer)
    {
        SolveDistanceConstraint();
        static bool bLoggedFallback = false;
        if (!bLoggedFallback)
        {
            XPBDN_LOG_INFO("Using CPU solver: GPU not available or buffers invalid.");
            bLoggedFallback = true;
        }
        return;
    }

    FRDGBuilder GraphBuilder(RHICmdList);

    FRDGBuffer* PositionsRDG = GraphBuilder.RegisterExternalBuffer(PositionsBuffer, TEXT("XPBDNPositions"));
    FRDGBuffer* ConstraintsRDG = GraphBuilder.RegisterExternalBuffer(ConstraintsBuffer, TEXT("XPBDNConstraints"));
    FRDGBuffer* RestLengthsRDG = GraphBuilder.RegisterExternalBuffer(RestLengthsBuffer, TEXT("XPBDNRestLengths"));

    FXPBDNComputeShader::FParameters Parameters;
    Parameters.Positions = GraphBuilder.CreateUAV(PositionsRDG);
    Parameters.Constraints = GraphBuilder.CreateSRV(ConstraintsRDG);
    Parameters.RestLengths = GraphBuilder.CreateSRV(RestLengthsRDG);

    ShaderManager.DispatchShader(RHICmdList, Parameters, Constraints.Num());
}

void FXPBDNMuscleSolver::SolveDistanceConstraint()
{
    for (int32 i = 0; i < Constraints.Num(); ++i)
    {
        int32 Idx1 = Constraints[i].X;
        int32 Idx2 = Constraints[i].Y;
        float RestLength = RestLengths[i];

        FVector& P1 = Positions[Idx1];
        FVector& P2 = Positions[Idx2];
        FVector Delta = P2 - P1;
        float CurrentLength = Delta.Size();

        if (CurrentLength > 0.0f)
        {
            float Correction = (CurrentLength - RestLength) * 0.5f;
            FVector CorrectionVector = Delta.GetSafeNormal() * Correction;
            P1 += CorrectionVector;
            P2 -= CorrectionVector;
        }
    }
}