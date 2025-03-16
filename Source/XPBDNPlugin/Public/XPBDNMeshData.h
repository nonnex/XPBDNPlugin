// XPBDNMeshData.h
#pragma once

#include "CoreMinimal.h"
#include "XPBDNMeshData.generated.h"

USTRUCT()
struct XPBDNPLUGIN_API FXPBDNMeshData
{
    GENERATED_BODY()

    TArray<FVector> Positions;
    TArray<int32> Constraints;  // Pairs or indices defining constraints
};
