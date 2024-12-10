// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Components/SceneComponent.h>

#include "CMGInteractionComponent.generated.h"

class UCMGInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCMGInteraction, const UCMGInteractorComponent*, InteractorComponent);

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class CMGGAMEPLAYFRAMEWORK_API UCMGInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UCMGInteractionComponent();

	// Begin UActorComponent Overrides
	void BeginPlay() override;
	// End UActorComponent Overrides
	
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void StartInteraction(const UCMGInteractorComponent* InteractorComponent);
	virtual void StartInteraction_Implementation(const UCMGInteractorComponent* InteractorComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void StopInteraction(const UCMGInteractorComponent* InteractorComponent);
	virtual void StopInteraction_Implementation(const UCMGInteractorComponent* InteractorComponent);

public:
	UPROPERTY(BlueprintAssignable)
	FCMGInteraction OnInteractionStart;

	UPROPERTY(BlueprintAssignable)
	FCMGInteraction OnInteractionEnd;
};
