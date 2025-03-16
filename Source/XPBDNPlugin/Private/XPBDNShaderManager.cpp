#include "XPBDNShaderManager.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"

IMPLEMENT_GLOBAL_SHADER(FXPBDNComputeShader, "/Plugins/XPBDNPlugin/Shaders/XPBDCompute.usf", "MainCS", SF_Compute);

FXPBDNShaderManager::FXPBDNShaderManager()
    : bShaderValid(false)
{
}

FXPBDNShaderManager::~FXPBDNShaderManager()
{
    ShaderInitFence.Wait();
}

bool FXPBDNShaderManager::Initialize()
{
    FString ShaderPath = FPaths::ProjectPluginsDir() / TEXT("XPBDNPlugin/Shaders/XPBDCompute.usf");
    static bool bLoggedShaderCheck = false;

    if (!FPaths::FileExists(ShaderPath))
    {
        if (!bLoggedShaderCheck)
        {
            XPBDN_LOG_ERROR("Shader file missing at %s. GPU acceleration disabled.", *ShaderPath);
            bLoggedShaderCheck = true;
        }
        bShaderValid = false;
        return false;
    }

    ENQUEUE_RENDER_COMMAND(InitShader)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            TShaderMapRef<FXPBDNComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
            if (ComputeShader.IsValid())
            {
                bShaderValid = true;
                static bool bLoggedSuccess = false;
                if (!bLoggedSuccess)
                {
                    XPBDN_LOG_INFO("XPBDN Compute Shader initialized successfully.");
                    bLoggedSuccess = true;
                }
            }
            else
            {
                XPBDN_LOG_ERROR("Shader compilation failed.");
                bShaderValid = false;
            }
        });
    ShaderInitFence.BeginFence();
    ShaderInitFence.Wait();

    return bShaderValid;
}

void FXPBDNShaderManager::DispatchShader(FRHICommandListImmediate& RHICmdList, const FXPBDNComputeShader::FParameters& Parameters, uint32 NumConstraints)
{
    if (!bShaderValid || !Parameters.Positions || !Parameters.Constraints || !Parameters.RestLengths)
    {
        static bool bLoggedDispatchFail = false;
        if (!bLoggedDispatchFail)
        {
            XPBDN_LOG_WARNING("Cannot dispatch shader: Shader invalid or parameters null.");
            bLoggedDispatchFail = true;
        }
        return;
    }

    FRDGBuilder GraphBuilder(RHICmdList);
    TShaderMapRef<FXPBDNComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("XPBDNCompute"),
        &Parameters,
        ERDGPassFlags::Compute,
        [&](FRHICommandListImmediate& RHICmdListInner)
        {
            FRHIComputeShader* ShaderRHI = ComputeShader.GetComputeShader();
            RHICmdListInner.SetComputeShader(ShaderRHI);

            RHICmdListInner.SetUAVParameter(ShaderRHI, 0, Parameters.Positions->GetRHI());
            RHICmdListInner.SetShaderResourceViewParameter(ShaderRHI, 0, Parameters.Constraints->GetRHI());
            RHICmdListInner.SetShaderResourceViewParameter(ShaderRHI, 1, Parameters.RestLengths->GetRHI());

            uint32 GroupCount = FMath::DivideAndRoundUp(NumConstraints, 64u);
            FComputeShaderUtils::Dispatch(RHICmdListInner, ComputeShader, Parameters, FIntVector(GroupCount, 1, 1));

            static bool bLoggedDispatch = false;
            if (!bLoggedDispatch)
            {
                XPBDN_LOG_INFO("Shader dispatched with %d constraints, %d groups.", NumConstraints, GroupCount);
                bLoggedDispatch = true;
            }
        });

    GraphBuilder.Execute();
}