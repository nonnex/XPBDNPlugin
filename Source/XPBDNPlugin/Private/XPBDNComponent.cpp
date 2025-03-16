// XPBDNComponent.cpp
#include "XPBDNComponent.h"
#include "XPBDNPlugin.h"  // For logging

UXPBDNComponent::UXPBDNComponent()
{
}

void UXPBDNComponent::BeginPlay()
{
    Super::BeginPlay();
    XPBDN_LOG_INFO("XPBDNComponent attached and ready!");
}