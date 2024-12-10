// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

#include <Modules/ModuleManager.h>
#include <NativeGameplayTags.h>

CMGGAMEPLAYFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Stat_Health)

class FCMGGameplayFrameworkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
