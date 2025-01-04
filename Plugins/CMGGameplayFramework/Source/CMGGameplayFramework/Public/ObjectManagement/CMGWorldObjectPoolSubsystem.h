// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Subsystems/WorldSubsystem.h>
#include <TimerManager.h>

#include "ObjectManagement/CMGObjectPool.h"
#include "ObjectManagement/CMGPoolableObject.h"

#include "CMGWorldObjectPoolSubsystem.generated.h"

class UPrimitiveComponent;

/**
 * WorldSubsystem in charge of handling the management of object pools
 */
UCLASS()
class CMGGAMEPLAYFRAMEWORK_API UCMGWorldObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void Deinitialize() override;

	// Begin UWorldSubsystem override
	void OnWorldBeginPlay(UWorld& InWorld) override;
	// End UWorldSubsystem override

	template<typename TObjectClass>
	void CreatePool(TSubclassOf<TObjectClass> ObjectClass, int32 PoolSize);

	template<typename TObjectClass>
	TObjectClass* GetOrCreateObject(TSubclassOf<TObjectClass> ObjectClass);

	template<typename TObjectClass>
	void ReleasePoolObject(TObjectClass* PoolObject);

private:
	void CleanupObjectPools();

	template<typename TObjectClass>
	TObjectClass* CreatePoolObject(
		FCMGObjectPool& ObjectPool, TSubclassOf<TObjectClass> ObjectClass, const bool bImmediatelyUsed = false);

	template<typename TObjectClass>
	void ResetPoolObject(TObjectClass* PoolObject);

	void CullInactiveObjects();

private:
	UPROPERTY()
	TMap<TSubclassOf<UObject>, FCMGObjectPool> ObjectPools;

	FTimerHandle CullHandle;
};

template<typename TObjectClass>
void UCMGWorldObjectPoolSubsystem::CreatePool(TSubclassOf<TObjectClass> ObjectClass, int32 PoolSize)
{
	UWorld* const World = GetWorld();
	if (World == nullptr || ObjectClass == nullptr || PoolSize <= 0)
	{
		return;
	}

	FCMGObjectPool& ObjectPool = ObjectPools.FindOrAdd(ObjectClass);
	ObjectPool.InitializeObjectPool(PoolSize);

	for (int32 Idx = 0; Idx < PoolSize; Idx++)
	{
		CreatePoolObject(ObjectPool, ObjectClass);
	}
}

template<typename TObjectClass>
TObjectClass* UCMGWorldObjectPoolSubsystem::GetOrCreateObject(
	TSubclassOf<TObjectClass> ObjectClass)
{
	if (FCMGObjectPool* FoundPool = ObjectPools.Find(ObjectClass))
	{
		TObjectClass* PoolObject = Cast<TObjectClass>(FoundPool->GetAvailableObject());
		if (PoolObject == nullptr)
		{
			const bool bImmediatelyUsed = true;
			PoolObject = CreatePoolObject(*FoundPool, ObjectClass, bImmediatelyUsed);
			if (PoolObject == nullptr)
			{
				return nullptr;
			}
		}

		if (PoolObject->IsA<AActor>())
		{
			PoolObject->SetActorHiddenInGame(false);
		}

		if (PoolObject->Implements<UCMGPoolableObject>())
		{
			ICMGPoolableObject::Execute_SetLastUsedTime(PoolObject, GetWorld()->GetTimeSeconds());
		}

		return PoolObject;
	}
	return nullptr;
}

template<typename TObjectClass>
void UCMGWorldObjectPoolSubsystem::ReleasePoolObject(TObjectClass* PoolObject)
{
	if (PoolObject == nullptr)
	{
		return;
	}

	FCMGObjectPool* FoundPool = ObjectPools.Find(PoolObject->GetClass());
	if (FoundPool == nullptr)
	{
		return;
	}
	ResetPoolObject(PoolObject);
	FoundPool->ReleaseObject(PoolObject);
}

template<typename TObjectClass>
TObjectClass* UCMGWorldObjectPoolSubsystem::CreatePoolObject(
	FCMGObjectPool& ObjectPool, TSubclassOf<TObjectClass> ObjectClass, const bool bImmediatelyUsed /*= false*/)
{
	UWorld* const World = GetWorld();
	if (World == nullptr || ObjectClass == nullptr)
	{
		return nullptr;
	}

	TObjectClass* PoolObject = nullptr;
	if (ObjectClass->IsChildOf(AActor::StaticClass()))
	{
		PoolObject = World->SpawnActor<TObjectClass>(ObjectClass);
		if (AActor* const ActorPoolObject = Cast<AActor>(PoolObject))
		{
			ResetPoolObject(ActorPoolObject);
		}
	}
	else
	{
		PoolObject = NewObject<TObjectClass>(World, ObjectClass);
		PoolObject->AddToRoot();
	}

	if (PoolObject != nullptr)
	{
		ObjectPool.AddObjectToPool(PoolObject, bImmediatelyUsed);
	}
	return PoolObject;
}

template<typename TObjectClass>
void UCMGWorldObjectPoolSubsystem::ResetPoolObject(TObjectClass* PoolObject)
{
	if (PoolObject == nullptr)
	{
		return;
	}
	if (AActor* const ActorPoolObject = Cast<AActor>(PoolObject))
	{
		ActorPoolObject->SetActorHiddenInGame(true);
		ActorPoolObject->SetActorLocation(FVector(0.f));

		// Disable collision for the actor whilst it's inactive in the pool
		if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(ActorPoolObject->GetRootComponent()))
		{
			RootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
