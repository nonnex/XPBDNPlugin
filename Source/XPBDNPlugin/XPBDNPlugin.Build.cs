// XPBDNPlugin.Build.cs
using UnrealBuildTool;

public class XPBDNPlugin : ModuleRules
{
    public XPBDNPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject", // For UObject, UPackage
            "Engine",      // For UActorComponent
            "RenderCore",  // For RDG
            "Renderer",    // For shader utils
            "RHI"          // For FRHICommandList, FRHIGPUBufferReadback
            // "ShaderCore" removed - deprecated in 5.5.4
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            // Add private dependencies if needed
        });
    }
}