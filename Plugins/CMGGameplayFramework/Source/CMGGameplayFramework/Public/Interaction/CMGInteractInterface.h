// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <UObject/Interface.h>

#include "CMGInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCMGInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class CMGGAMEPLAYFRAMEWORK_API ICMGInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interact|Info")
	FString GetInteractionName() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Interact|Focus")
	void UpdateFocus(const bool bIsInFocus);

	UFUNCTION(BlueprintNativeEvent, Category = "Interact|Focus")
	bool CanInteract();
};
