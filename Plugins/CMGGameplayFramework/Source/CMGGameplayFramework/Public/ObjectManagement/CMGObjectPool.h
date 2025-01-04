// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include "CMGObjectPool.generated.h"

class UWorld;

USTRUCT()
struct CMGGAMEPLAYFRAMEWORK_API FCMGObjectPool
{
	GENERATED_BODY()

public:
	void InitializeObjectPool(int32 PoolSize);

	void AddObjectToPool(UObject* Object, const bool bImmediatelyUsed = false);
	void ResetPool();

	void CullInactiveObjects(const UWorld& World);

	UObject* GetAvailableObject();
	void ReleaseObject(UObject* Object);

	TArray<UObject*> GetAllPooledObjects() const;

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> ActivePooledObjects;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> InactivePooledObjects;

	UPROPERTY(Transient)
	TObjectPtr<UClass> PoolClass;

	int32 DefaultSize = INDEX_NONE;
};

inline TArray<UObject*> FCMGObjectPool::GetAllPooledObjects() const
{
	TArray<UObject*> AllObjects;
	AllObjects.Append(ActivePooledObjects);
	AllObjects.Append(InactivePooledObjects);
	return AllObjects;
}
