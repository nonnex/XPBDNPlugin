// XPBDNPlugin.cpp
#include "XPBDNPlugin.h"

// Define the log category
DEFINE_LOG_CATEGORY(LogXPBDNPlugin);

void FXPBDNPluginModule::StartupModule()
{
    // Log a test message during plugin initialization
    XPBDN_LOG_INFO("XPBDNPlugin initialized successfully!");

    // Read configuration example
    FString Verbosity;
    if (GConfig->GetString(TEXT("XPBDNPlugin"), TEXT("LogVerbosity"), Verbosity, GGameIni))
    {
        XPBDN_LOG_INFO("Log verbosity set to: %s", *Verbosity);
    }
    else
    {
        XPBDN_LOG_WARNING("LogVerbosity not found in config.");
    }
}

void FXPBDNPluginModule::ShutdownModule()
{
    XPBDN_LOG_INFO("XPBDNPlugin shutdown.");
}

IMPLEMENT_MODULE(FXPBDNPluginModule, XPBDNPlugin)