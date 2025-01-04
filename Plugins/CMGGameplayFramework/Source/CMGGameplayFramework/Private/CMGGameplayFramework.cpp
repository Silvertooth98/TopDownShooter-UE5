// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMGGameplayFramework.h"

#include <ISettingsModule.h>
#include <Modules/ModuleManager.h>

#include "ObjectManagement/CMGObjectManagementSettings.h"

#define LOCTEXT_NAMESPACE "FCMGGameplayFrameworkModule"

UE_DEFINE_GAMEPLAY_TAG(TAG_Stat_Health, "Stat.Health");

void FCMGGameplayFrameworkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"CMGGameplayFramework_Settings",
			LOCTEXT("RuntimeSettingsName", "CMGGameplayFramework"),
			LOCTEXT("RuntimeSettingsDescription", "Configure your plugin settings"),
			GetMutableDefault<UCMGObjectManagementSettings>());
	}
}

void FCMGGameplayFrameworkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(
			"Project",
			"Plugins",
			"CMGGameplayFramework_Settings");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCMGGameplayFrameworkModule, CMGGameplayFramework)