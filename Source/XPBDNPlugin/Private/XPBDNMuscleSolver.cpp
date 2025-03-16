// XPBDNMuscleSolver.cpp
#include "XPBDNMuscleSolver.h"
#include "XPBDNMeshData.h"
#include "XPBDNPlugin.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphUtils.h"
#include "RHIUtilities.h"
#include "RenderingThread.h"
#include "Misc/Paths.h" // For FPaths::FileExists

class FXPBDComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FXPBDComputeShader);
    SHADER_USE_PARAMETER_STRUCT(FXPBDComputeShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Positions)
        SHADER_PARAMETER_SRV(StructuredBuffer<int2>, Constraints)
        SHADER_PARAMETER_SRV(StructuredBuffer<float>, RestDistances)
        SHADER_PARAMETER(float, Stiffness)
        SHADER_PARAMETER(float, DeltaTime)
        SHADER_PARAMETER(uint32, NumConstraints)
        SHADER_PARAMETER(uint32, NumParticles)
    END_SHADER_PARAMETER_STRUCT()

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE"), 64);
    }
};

IMPLEMENT_GLOBAL_SHADER(FXPBDComputeShader, "/Plugins/XPBDNPlugin/Shaders/XPBDCompute.usf", "CSMain", SF_Compute);

void FXPBDNMuscleSolver::InitializeSolver(const FXPBDNMeshData& Data)
{
    if (Data.Positions.IsEmpty() || Data.RestPositions.IsEmpty())
    {
        XPBDN_LOG_ERROR("No positions or rest positions provided to solver!");
        return;
    }
    if (Data.Positions.Num() != Data.RestPositions.Num())
    {
        XPBDN_LOG_ERROR("Mismatch between positions (%d) and rest positions (%d)!", Data.Positions.Num(), Data.RestPositions.Num());
        return;
    }
    MeshData = Data;

    // Check shader file early to avoid static compilation crash
    FString ShaderPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("XPBDNPlugin/Shaders/XPBDCompute.usf"));
    bUseGPU = FPaths::FileExists(ShaderPath);
    if (!bUseGPU)
    {
        XPBDN_LOG_ERROR("Shader file '%s' not found! Using CPU solver to prevent runtime crash. Ensure the .usf file is in place.", *ShaderPath);
    }
    else
    {
        XPBDN_LOG_INFO("Shader file '%s' found. GPU solver enabled.", *ShaderPath);
    }

    if (bUseGPU)
    {
        ENQUEUE_RENDER_COMMAND(SetupBuffers)(
            [this](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder GraphBuilder(RHICmdList);
                SetupGPUBuffers(GraphBuilder);
                GraphBuilder.Execute();
            });
    }
    XPBDN_LOG_INFO("Solver initialized with %d vertices", MeshData.Positions.Num());
}

void FXPBDNMuscleSolver::SolveConstraints(FRDGBuilder& GraphBuilder, float DeltaTime)
{
    if (!bUseGPU)
    {
        for (int32 i = 0; i < MeshData.Constraints.Num(); i += 2)
        {
            SolveDistanceConstraint(i / 2, 1.0f, DeltaTime);
        }
        XPBDN_LOG_INFO("Solved %d distance constraints (CPU) with DeltaTime: %f", MeshData.Constraints.Num() / 2, DeltaTime);
        return;
    }

    if (!PositionsBuffer || !ConstraintsBuffer || !RestDistancesBuffer)
    {
        XPBDN_LOG_ERROR("One or more GPU buffers are null! Positions=%p, Constraints=%p, RestDistances=%p",
            PositionsBuffer, ConstraintsBuffer, RestDistancesBuffer);
        return;
    }

    static bool bLoggedInitialPositions = false;
    if (!bLoggedInitialPositions)
    {
        for (int32 i = 0; i < MeshData.Positions.Num(); ++i)
        {
            XPBDN_LOG_VERBOSE("Initial Position %d: %s", i, *MeshData.Positions[i].ToString());
        }
        bLoggedInitialPositions = true;
    }

    DispatchDistanceConstraints(GraphBuilder, DeltaTime);

    PositionsPooledBuffer = nullptr;
    GraphBuilder.QueueBufferExtraction(PositionsBuffer, &PositionsPooledBuffer);
    XPBDN_LOG_VERBOSE("Queued buffer extraction for %d positions", MeshData.Positions.Num());

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("ReadbackPositions"),
        ERDGPassFlags::Readback,
        [this](FRHICommandListImmediate& RHICmdList)
        {
            if (!PositionsPooledBuffer.IsValid())
            {
                XPBDN_LOG_ERROR("PositionsPooledBuffer is null or invalid after extraction!");
                return;
            }

            FRHIGPUBufferReadback Readback(TEXT("PositionsReadback"));
            Readback.EnqueueCopy(RHICmdList, PositionsPooledBuffer->GetRHI(), MeshData.Positions.Num() * sizeof(FVector));
            XPBDN_LOG_VERBOSE("Enqueued readback for %d bytes", MeshData.Positions.Num() * sizeof(FVector));

            RHICmdList.BlockUntilGPUIdle();
            void* Data = Readback.Lock(MeshData.Positions.Num() * sizeof(FVector));
            if (Data)
            {
                FMemory::Memcpy(MeshData.Positions.GetData(), Data, MeshData.Positions.Num() * sizeof(FVector));
                Readback.Unlock();

                static bool bLoggedFinalPositions = false;
                if (!bLoggedFinalPositions)
                {
                    for (int32 i = 0; i < MeshData.Positions.Num(); ++i)
                    {
                        XPBDN_LOG_VERBOSE("Final Position %d: %s", i, *MeshData.Positions[i].ToString());
                    }
                    bLoggedFinalPositions = true;
                }
            }
            else
            {
                XPBDN_LOG_ERROR("Failed to lock readback buffer - no data returned!");
            }
        });

    XPBDN_LOG_INFO("Solved %d distance constraints (GPU) with DeltaTime: %f", MeshData.Constraints.Num() / 2, DeltaTime);
}

void FXPBDNMuscleSolver::SolveDistanceConstraint(int32 ConstraintIdx, float Stiffness, float DeltaTime)
{
    int32 Idx1 = MeshData.Constraints[ConstraintIdx * 2];
    int32 Idx2 = MeshData.Constraints[ConstraintIdx * 2 + 1];
    FVector P1 = MeshData.Positions[Idx1];
    FVector P2 = MeshData.Positions[Idx2];
    float RestDistance = (MeshData.RestPositions[Idx2] - MeshData.RestPositions[Idx1]).Size();

    FVector Delta = P2 - P1;
    float CurrentDistance = Delta.Size();
    if (CurrentDistance > 0.0f)
    {
        float Correction = (CurrentDistance - RestDistance) * Stiffness * DeltaTime;
        FVector CorrectionVector = Delta.GetSafeNormal() * (Correction * 0.5f);
        MeshData.Positions[Idx1] -= CorrectionVector;
        MeshData.Positions[Idx2] += CorrectionVector;
    }
}

void FXPBDNMuscleSolver::DispatchShader()
{
    XPBDN_LOG_INFO("Shader dispatched!");
}

void FXPBDNMuscleSolver::SetupGPUBuffers(FRDGBuilder& GraphBuilder)
{
    if (MeshData.Positions.Num() <= 0 || MeshData.Constraints.Num() <= 0)
    {
        XPBDN_LOG_ERROR("Invalid mesh data for GPU buffer setup: Positions=%d, Constraints=%d", MeshData.Positions.Num(), MeshData.Constraints.Num());
        return;
    }

    FRDGBufferDesc PositionsDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector), MeshData.Positions.Num());
    PositionsDesc.Usage = EBufferUsageFlags::Dynamic | EBufferUsageFlags::UnorderedAccess | EBufferUsageFlags::ShaderResource;
    PositionsBuffer = GraphBuilder.CreateBuffer(PositionsDesc, TEXT("PositionsBuffer"));
    if (!PositionsBuffer)
    {
        XPBDN_LOG_ERROR("Failed to create PositionsBuffer!");
        return;
    }
    GraphBuilder.QueueBufferUpload(PositionsBuffer, MeshData.Positions.GetData(), MeshData.Positions.Num() * sizeof(FVector));
    XPBDN_LOG_VERBOSE("Uploaded %d positions to GPU", MeshData.Positions.Num());

    FRDGBufferDesc ConstraintsDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(int32) * 2, MeshData.Constraints.Num() / 2);
    ConstraintsDesc.Usage = EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource;
    ConstraintsBuffer = GraphBuilder.CreateBuffer(ConstraintsDesc, TEXT("ConstraintsBuffer"));
    if (!ConstraintsBuffer)
    {
        XPBDN_LOG_ERROR("Failed to create ConstraintsBuffer!");
        return;
    }
    GraphBuilder.QueueBufferUpload(ConstraintsBuffer, MeshData.Constraints.GetData(), MeshData.Constraints.Num() * sizeof(int32));
    XPBDN_LOG_VERBOSE("Uploaded %d constraints to GPU", MeshData.Constraints.Num() / 2);

    TArray<float> RestDistances;
    RestDistances.SetNum(MeshData.Constraints.Num() / 2);
    for (int32 i = 0; i < MeshData.Constraints.Num(); i += 2)
    {
        int32 Idx1 = MeshData.Constraints[i];
        int32 Idx2 = MeshData.Constraints[i + 1];
        RestDistances[i / 2] = (MeshData.RestPositions[Idx2] - MeshData.RestPositions[Idx1]).Size();
    }
    FRDGBufferDesc RestDistancesDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(float), RestDistances.Num());
    RestDistancesDesc.Usage = EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource;
    RestDistancesBuffer = GraphBuilder.CreateBuffer(RestDistancesDesc, TEXT("RestDistancesBuffer"));
    if (!RestDistancesBuffer)
    {
        XPBDN_LOG_ERROR("Failed to create RestDistancesBuffer!");
        return;
    }
    GraphBuilder.QueueBufferUpload(RestDistancesBuffer, RestDistances.GetData(), RestDistances.Num() * sizeof(float));
    XPBDN_LOG_VERBOSE("Uploaded %d rest distances to GPU", RestDistances.Num());

    XPBDN_LOG_INFO("GPU buffers set up: %d positions, %d constraints", MeshData.Positions.Num(), MeshData.Constraints.Num() / 2);
}

void FXPBDNMuscleSolver::DispatchDistanceConstraints(FRDGBuilder& GraphBuilder, float DeltaTime)
{
    if (!bUseGPU)
    {
        XPBDN_LOG_WARNING("Skipping GPU dispatch - shader unavailable.");
        return;
    }

    if (!PositionsBuffer || !ConstraintsBuffer || !RestDistancesBuffer)
    {
        XPBDN_LOG_ERROR("One or more GPU buffers are null! Positions=%p, Constraints=%p, RestDistances=%p",
            PositionsBuffer, ConstraintsBuffer, RestDistancesBuffer);
        return;
    }

    // Scope shader access to delay compilation until we're sure
    TShaderMapRef<FXPBDComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
    if (!ComputeShader.IsValid())
    {
        XPBDN_LOG_ERROR("ComputeShader is invalid! Check XPBDCompute.usf at '/Plugins/XPBDNPlugin/Shaders/'. Falling back to CPU.");
        bUseGPU = false; // Disable GPU for future calls
        return;
    }

    FXPBDComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FXPBDComputeShader::FParameters>();
    if (!PassParameters)
    {
        XPBDN_LOG_ERROR("Failed to allocate PassParameters!");
        return;
    }

    FRDGBufferUAVRef PositionsUAV = GraphBuilder.CreateUAV(PositionsBuffer);
    FRDGBufferSRVRef ConstraintsSRV = GraphBuilder.CreateSRV(ConstraintsBuffer);
    FRDGBufferSRVRef RestDistancesSRV = GraphBuilder.CreateSRV(RestDistancesBuffer);
    if (!PositionsUAV || !ConstraintsSRV || !RestDistancesSRV)
    {
        XPBDN_LOG_ERROR("Failed to create UAV/SRV: PositionsUAV=%p, ConstraintsSRV=%p, RestDistancesSRV=%p",
            PositionsUAV, ConstraintsSRV, RestDistancesSRV);
        return;
    }

    PassParameters->Positions = PositionsUAV->GetRHI();
    PassParameters->Constraints = ConstraintsSRV->GetRHI();
    PassParameters->RestDistances = RestDistancesSRV->GetRHI();
    PassParameters->Stiffness = 1.0f;
    PassParameters->DeltaTime = DeltaTime;
    PassParameters->NumConstraints = MeshData.Constraints.Num() / 2;
    PassParameters->NumParticles = MeshData.Positions.Num();

    uint32 GroupCountX = FMath::DivideAndRoundUp(PassParameters->NumConstraints, 64u);
    GraphBuilder.AddPass(
        RDG_EVENT_NAME("XPBDComputeDispatch"),
        PassParameters,
        ERDGPassFlags::Compute,
        [this, ComputeShader, PassParameters, GroupCountX](FRHICommandList& RHICmdList)
        {
            FRHIComputeCommandList& ComputeCmdList = RHICmdList;
            TArray<FRHITransitionInfo> Transitions;
            Transitions.Add(FRHITransitionInfo(PassParameters->Positions, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
            ComputeCmdList.Transition(Transitions);
            FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, FIntVector(GroupCountX, 1, 1));
            XPBDN_LOG_VERBOSE("Dispatched compute shader with %d thread groups", GroupCountX);
        });

    XPBDN_LOG_INFO("Dispatched GPU distance constraints for %d constraints", PassParameters->NumConstraints);
}