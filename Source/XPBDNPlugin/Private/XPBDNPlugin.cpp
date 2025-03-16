// XPBDNPlugin.cpp
#include "XPBDNPlugin.h"

// Define the log category
DEFINE_LOG_CATEGORY(LogXPBDNPlugin);

void FXPBDNPluginModule::StartupModule()
{
    // Log a test message during plugin initialization
    XPBDN_LOG_INFO("XPBDNPlugin initialized successfully!");

    // Explicitly load from the plugin's DefaultXPBDNPlugin.ini
    FString ConfigPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("XPBDNPlugin/Config/DefaultXPBDNPlugin.ini"));
    FString Verbosity;
    if (GConfig->GetString(TEXT("XPBDNPlugin"), TEXT("LogVerbosity"), Verbosity, ConfigPath))
    {
        XPBDN_LOG_INFO("Log verbosity set to: %s", *Verbosity);
    }
    else
    {
        XPBDN_LOG_WARNING("LogVerbosity not found in config at: %s", *ConfigPath);
    }
}

void FXPBDNPluginModule::ShutdownModule()
{
    XPBDN_LOG_INFO("XPBDNPlugin shutdown.");
}

IMPLEMENT_MODULE(FXPBDNPluginModule, XPBDNPlugin)