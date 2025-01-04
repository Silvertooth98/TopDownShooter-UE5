// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectManagement/CMGObjectPool.h"

#include <Engine/World.h>

#include "ObjectManagement/CMGObjectManagementSettings.h"
#include "ObjectManagement/CMGPoolableObject.h"

void FCMGObjectPool::InitializeObjectPool(int32 PoolSize)
{
	ActivePooledObjects.Reserve(PoolSize);
	InactivePooledObjects.Reserve(PoolSize);
	DefaultSize = PoolSize;
}

void FCMGObjectPool::AddObjectToPool(UObject* Object, const bool bImmediatelyUsed /*= false*/)
{
	if (Object == nullptr)
	{
		return;
	}
	if (PoolClass == nullptr)
	{
		PoolClass = Object->GetClass();
	}
	if (Object->GetClass() != PoolClass)
	{
		return;
	}
	// Add the new pool object to the correct container within the pool depending on if it's being used on creation or not
	bImmediatelyUsed ? ActivePooledObjects.Add(Object) : InactivePooledObjects.Add(Object);
}

void FCMGObjectPool::ResetPool()
{
	for (UObject* PooledObject : GetAllPooledObjects())
	{
		if (PooledObject == nullptr || !IsValid(PooledObject))
		{
			continue;
		}
		if (AActor* PooledActor = Cast<AActor>(PooledObject))
		{
			PooledActor->Destroy();
		}
		else
		{
			PooledObject->RemoveFromRoot();
			PooledObject->ConditionalBeginDestroy();
		}
	}
	ActivePooledObjects.Reset(DefaultSize);
	InactivePooledObjects.Reset(DefaultSize);
}

void FCMGObjectPool::CullInactiveObjects(const UWorld& World)
{
	if (InactivePooledObjects.Num() <= 0 || InactivePooledObjects.Num() <= DefaultSize)
	{
		return;
	}

	const UCMGObjectManagementSettings* Settings = GetDefault<UCMGObjectManagementSettings>();
	if (Settings == nullptr)
	{
		return;
	}

	const float CurrentTimeSeconds = World.GetTimeSeconds();
	const float CullThreshold = Settings->GetPooledObjectCullThreshold();
	for (int32 Idx = InactivePooledObjects.Num() - 1; Idx >= 0; Idx--)
	{
		const int32 InactivePooledObjectsNum = InactivePooledObjects.Num();
		if (InactivePooledObjectsNum <= DefaultSize)
		{
			break;
		}

		UObject* InactiveObject = InactivePooledObjects[Idx];
		if (!InactiveObject->Implements<UCMGPoolableObject>())
		{
			continue;
		}
		const float TimeLastUsed = ICMGPoolableObject::Execute_GetLastUsedTime(InactiveObject);
		const float TimeSinceLastUsed = CurrentTimeSeconds - TimeLastUsed;
		if (TimeSinceLastUsed >= CullThreshold)
		{
			if (AActor* InactiveActor = Cast<AActor>(InactiveObject))
			{
				InactiveActor->Destroy();
			}
			else
			{
				InactiveObject->RemoveFromRoot();
				InactiveObject->ConditionalBeginDestroy();
			}
			InactivePooledObjects.RemoveAt(Idx);
		}
	}
}

UObject* FCMGObjectPool::GetAvailableObject()
{
	if (InactivePooledObjects.Num() <= 0)
	{
		return nullptr;
	}

	UObject* AvailableObject = InactivePooledObjects.Pop();
	ActivePooledObjects.AddUnique(AvailableObject);
	return AvailableObject;
}

void FCMGObjectPool::ReleaseObject(UObject* Object)
{
	if (Object == nullptr || Object->GetClass() != PoolClass)
	{
		return;
	}
	InactivePooledObjects.AddUnique(Object);
	ActivePooledObjects.RemoveSingle(Object);
}
