// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectManagement/CMGWorldObjectPoolSubsystem.h"

#include <Engine/World.h>

#include "ObjectManagement/CMGObjectManagementSettings.h"

void UCMGWorldObjectPoolSubsystem::Deinitialize()
{
	CleanupObjectPools();

	Super::Deinitialize();
}

void UCMGWorldObjectPoolSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UCMGObjectManagementSettings* Settings = GetDefault<UCMGObjectManagementSettings>();
	if (Settings == nullptr)
	{
		return;
	}

	for (const auto& Elem : Settings->GetRequestedPools())
	{
		CreatePool(Elem.Key, Elem.Value);
	}

	if (Settings->ShouldCullInactivePooledObjectsAfterThreshold())
	{
		InWorld.GetTimerManager().SetTimer(
			CullHandle,
			this,
			&ThisClass::CullInactiveObjects,
			Settings->GetTimeBetweenCullingInactivePooledObjects(),
			/*bLooping*/ true);
	}
}

void UCMGWorldObjectPoolSubsystem::CleanupObjectPools()
{
	for (auto& ObjectPool : ObjectPools)
	{
		ObjectPool.Value.ResetPool();
	}
	ObjectPools.Empty();
}

void UCMGWorldObjectPoolSubsystem::CullInactiveObjects()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	for (auto& ObjectPool : ObjectPools)
	{
		ObjectPool.Value.CullInactiveObjects(*World);
	}
}
