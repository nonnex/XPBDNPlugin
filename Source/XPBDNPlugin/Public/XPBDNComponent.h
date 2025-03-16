// XPBDNComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XPBDNComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class XPBDNPLUGIN_API UXPBDNComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UXPBDNComponent();

    virtual void BeginPlay() override;
};