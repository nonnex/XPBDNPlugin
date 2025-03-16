// XPBDNPlugin.h
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Declare the log category
DECLARE_LOG_CATEGORY_EXTERN(LogXPBDNPlugin, Log, All);

// Logging macros
#define XPBDN_LOG_INFO(FMT, ...) UE_LOG(LogXPBDNPlugin, Log, TEXT(FMT), ##__VA_ARGS__)
#define XPBDN_LOG_WARNING(FMT, ...) UE_LOG(LogXPBDNPlugin, Warning, TEXT(FMT), ##__VA_ARGS__)
#define XPBDN_LOG_ERROR(FMT, ...) UE_LOG(LogXPBDNPlugin, Error, TEXT(FMT), ##__VA_ARGS__)

class FXPBDNPluginModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};