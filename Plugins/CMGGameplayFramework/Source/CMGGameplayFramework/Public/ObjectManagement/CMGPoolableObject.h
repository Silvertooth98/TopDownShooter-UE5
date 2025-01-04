// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <UObject/Interface.h>

#include "CMGPoolableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCMGPoolableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * Inherit from this on objects designed to be managed by an object pool within the UCMGWorldObjectPoolSubsystem
 */
class CMGGAMEPLAYFRAMEWORK_API ICMGPoolableObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "CMGGameplayFramework|Object Management|Poolable Objects")
	void SetLastUsedTime(float Time);
	virtual void SetLastUsedTime_Implementation(float Time) = 0;
	UFUNCTION(BlueprintNativeEvent, Category = "CMGGameplayFramework|Object Management|Poolable Objects")
	float GetLastUsedTime() const;
	virtual float GetLastUsedTime_Implementation() const = 0;
};
