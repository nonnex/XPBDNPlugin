// Shaders/XPBDCompute.hlsl
RWStructuredBuffer<float3> Positions : register(u0);

[numthreads(64, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    Positions[DTid.x] = float3(0, 0, 0);  // Placeholder
}