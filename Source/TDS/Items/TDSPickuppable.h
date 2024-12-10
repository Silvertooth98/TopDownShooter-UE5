// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Actor.h>

#include "TDSPickuppable.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class TDS_API ATDSPickuppable : public AActor
{
	GENERATED_BODY()

public:
	ATDSPickuppable();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupObjectMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PickupCollision;
};
