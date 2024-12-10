// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/CMGInteractionComponent.h"

#include "Interaction/CMGInteractorComponent.h"

UCMGInteractionComponent::UCMGInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCMGInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCMGInteractionComponent::StartInteraction_Implementation(const UCMGInteractorComponent* InteractorComponent)
{
	if (!InteractorComponent)
	{
		return;
	}
	OnInteractionStart.Broadcast(InteractorComponent);
}

void UCMGInteractionComponent::StopInteraction_Implementation(const UCMGInteractorComponent* InteractorComponent)
{
	if (!InteractorComponent)
	{
		return;
	}
	OnInteractionEnd.Broadcast(InteractorComponent);
}
