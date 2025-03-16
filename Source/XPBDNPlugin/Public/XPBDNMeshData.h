// XPBDNMeshData.h
#pragma once

#include "CoreMinimal.h"
#include "XPBDNMeshData.generated.h"

USTRUCT()
struct XPBDNPLUGIN_API FXPBDNMeshData
{
    GENERATED_BODY()

    TArray<FVector> Positions;         // Current positions
    TArray<FVector> RestPositions;     // Original rest positions for shape-matching
    TArray<int32> Constraints;         // Pairs or indices defining distance constraints
};