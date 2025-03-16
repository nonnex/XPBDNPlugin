// XPBDNComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XPBDNMeshData.h" // Added for FXPBDNMeshData
#include "XPBDNMuscleSolver.h"
#include "XPBDNPlugin.h"   // For logging
#include "XPBDNComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class XPBDNPLUGIN_API UXPBDNComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UXPBDNComponent();
    virtual ~UXPBDNComponent(); // Added destructor to clean up Solver

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FXPBDNMuscleSolver* Solver;
    FXPBDNMeshData MeshData; // Moved from local scope to member for persistence
};