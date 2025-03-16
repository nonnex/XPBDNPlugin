#pragma once

#include "CoreMinimal.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameters.h"
#include "RenderCommandFence.h"
#include "XPBDNPlugin.h"

class FXPBDNComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FXPBDNComputeShader);
    SHADER_USE_PARAMETER_STRUCT(FXPBDNComputeShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_UAV(RWStructuredBuffer<FVector>, Positions)
        SHADER_PARAMETER_SRV(StructuredBuffer<FIntPoint>, Constraints)
        SHADER_PARAMETER_SRV(StructuredBuffer<float>, RestLengths)
    END_SHADER_PARAMETER_STRUCT()

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
};

class FXPBDNShaderManager
{
public:
    FXPBDNShaderManager();
    ~FXPBDNShaderManager();

    bool Initialize();
    bool IsShaderValid() const { return bShaderValid; }
    void DispatchShader(FRHICommandListImmediate& RHICmdList, const FXPBDNComputeShader::FParameters& Parameters, uint32 NumConstraints);

private:
    bool bShaderValid;
    FRenderCommandFence ShaderInitFence;
};