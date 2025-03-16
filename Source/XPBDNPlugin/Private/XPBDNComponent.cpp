// XPBDNComponent.cpp
#include "XPBDNComponent.h"
#include "XPBDNMeshData.h"
#include "XPBDNPlugin.h" // For logging
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"

UXPBDNComponent::UXPBDNComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    Solver = nullptr;
}

UXPBDNComponent::~UXPBDNComponent()
{
    if (Solver)
    {
        delete Solver;
        Solver = nullptr;
        XPBDN_LOG_VERBOSE("Solver deleted in component destructor");
    }
}

void UXPBDNComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!Solver)
    {
        Solver = new FXPBDNMuscleSolver();
        XPBDN_LOG_VERBOSE("Solver allocated in BeginPlay");
    }

    // Initialize MeshData (persistent member now)
    MeshData.Positions = { FVector(0, 0, 0), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 2) };
    MeshData.RestPositions = MeshData.Positions;
    MeshData.Constraints = { 0, 1, 1, 2, 2, 0, 0, 3, 1, 3, 2, 3 }; // Tetrahedron

    if (Solver)
    {
        Solver->InitializeSolver(MeshData);
        XPBDN_LOG_INFO("Component initialized with %d vertices and %d constraints", MeshData.Positions.Num(), MeshData.Constraints.Num() / 2);
    }
    else
    {
        XPBDN_LOG_ERROR("Solver is null after allocation in BeginPlay!");
    }
}

void UXPBDNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Solver)
    {
        XPBDN_LOG_ERROR("Solver is null in TickComponent!");
        return;
    }

    ENQUEUE_RENDER_COMMAND(SolveConstraints)(
        [this, DeltaTime](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            Solver->SolveConstraints(GraphBuilder, DeltaTime);
            GraphBuilder.Execute();
        });

    XPBDN_LOG_VERBOSE("Tick processed with DeltaTime: %f", DeltaTime);
}