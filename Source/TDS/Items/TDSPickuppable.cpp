// Fill out your copyright notice in the Description page of Project Settings.

#include "TDSPickuppable.h"

#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>

ATDSPickuppable::ATDSPickuppable()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupObject"));
	check(PickupObjectMeshComponent);
	SetRootComponent(PickupObjectMeshComponent);
	PickupObjectMeshComponent->SetSimulatePhysics(true);

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	check(PickupCollision);
	PickupCollision->SetupAttachment(PickupObjectMeshComponent);
	PickupCollision->SetSphereRadius(100.f);
}
