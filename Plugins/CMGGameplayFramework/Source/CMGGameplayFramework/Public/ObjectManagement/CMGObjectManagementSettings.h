// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Engine/DeveloperSettings.h>

#include "CMGObjectManagementSettings.generated.h"

UCLASS(Config = Game, defaultconfig, Meta = (DisplayName = "Object Management Settings"))
class CMGGAMEPLAYFRAMEWORK_API UCMGObjectManagementSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// Begin UDeveloperSettings override
	bool SupportsAutoRegistration() const override { return false; }
	// End UDeveloperSettings override

	const TMap<TSubclassOf<UObject>, int32>& GetRequestedPools() const;
	bool ShouldCullInactivePooledObjectsAfterThreshold() const;
	float GetTimeBetweenCullingInactivePooledObjects() const;
	float GetPooledObjectCullThreshold() const;

private:
	UPROPERTY(Config, EditAnywhere, Category = "Object Pooling", Meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<UObject>, int32> RequestedPools;

	UPROPERTY(Config, EditAnywhere, Category = "Object Pooling", Meta = (AllowPrivateAccess = true))
	bool bCullInactivePooledObjectsAfterThreshold = true;

	// How often should the UCMGWorldObjectPoolSubsystem attempt to cull inactive pool objects that haven't been used over the PooledObjectCullThreshold
	UPROPERTY(
		Config,
		EditAnywhere,
		Category = "Object Pooling",
		Meta = (AllowPrivateAccess = true, EditCondition = "bCullInactivePooledObjectsAfterThreshold", EditConditionHides))
	float TimeBetweenCullingInactivePooledObjects = 30.f;

	// How long a pooled object can be inactive for before being culled by the UCMGWorldObjectPoolSubsystem
	UPROPERTY(
		Config,
		EditAnywhere,
		Category = "Object Pooling",
		Meta = (AllowPrivateAccess = true, EditCondition = "bCullInactivePooledObjectsAfterThreshold", EditConditionHides))
	float PooledObjectCullThreshold = 30.f;
};

inline const TMap<TSubclassOf<UObject>, int32>& UCMGObjectManagementSettings::GetRequestedPools() const
{
	return RequestedPools;
}

inline bool UCMGObjectManagementSettings::ShouldCullInactivePooledObjectsAfterThreshold() const
{
	return bCullInactivePooledObjectsAfterThreshold;
}

inline float UCMGObjectManagementSettings::GetTimeBetweenCullingInactivePooledObjects() const
{
	return TimeBetweenCullingInactivePooledObjects;
}

inline float UCMGObjectManagementSettings::GetPooledObjectCullThreshold() const
{
	return PooledObjectCullThreshold;
}
