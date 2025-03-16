// XPBDNMuscleSolver.h
#pragma once

#include "CoreMinimal.h"
#include "XPBDNMeshData.h"

class FXPBDNMuscleSolver
{
public:
    FXPBDNMeshData MeshData;

    void InitializeSolver(const FXPBDNMeshData& Data);
    void SolveConstraints(float DeltaTime);
    void DispatchShader();
};