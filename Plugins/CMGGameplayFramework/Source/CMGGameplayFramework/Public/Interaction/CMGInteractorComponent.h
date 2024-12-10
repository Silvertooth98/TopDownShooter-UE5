// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>

#include "CMGInteractorComponent.generated.h"

class AActor;

UCLASS(Blueprintable)
class CMGGAMEPLAYFRAMEWORK_API UCMGInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCMGInteractorComponent();

	// Begin UActorComponent Overrides
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Overrides

	void Interact();

private:
	void FocusTrace();

protected:
	UPROPERTY(EditDefaultsOnly)
	float MaxFocusDistance { 500.f };

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> FocusedActor { nullptr };
};
