// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/CMGInteractorComponent.h"

#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>

#include "Interaction/CMGInteractInterface.h"
#include "Interaction/CMGInteractionComponent.h"

// Sets default values for this component's properties
UCMGInteractorComponent::UCMGInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.03f;
}

void UCMGInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}
	if (PawnOwner->IsLocallyControlled())
	{
		constexpr bool bEnable = true;
		SetComponentTickEnabled(bEnable);
	}
}

void UCMGInteractorComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FocusTrace();
}

void UCMGInteractorComponent::Interact()
{
	if (!FocusedActor.IsValid())
	{
		return;
	}
	
	UCMGInteractionComponent* InteractionComp = FocusedActor->FindComponentByClass<UCMGInteractionComponent>();
	if (!InteractionComp)
	{
		return;
	}
	InteractionComp->StartInteraction(this);
}

void UCMGInteractorComponent::FocusTrace()
{
	const UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	const AActor* Owner = GetOwner();
	const AController* Controller = Owner != nullptr ? Owner->GetInstigatorController() : nullptr;
	if (!Controller)
	{
		return;
	}

	FVector Loc;
	FRotator Rot;
	Controller->GetPlayerViewPoint(Loc, Rot);

	const FVector Start = Loc;
	const FVector End = Start + (Rot.Vector() * MaxFocusDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		if (AActor* InteractableActor = HitResult.GetActor())
		{
			if (InteractableActor == FocusedActor)
			{
				return;
			}

			if (FocusedActor.IsValid())
			{
				constexpr bool bIsFocused = false;
				ICMGInteractInterface::Execute_UpdateFocus(FocusedActor.Get(), bIsFocused);
				FocusedActor = nullptr;
			}

			if (!InteractableActor->Implements<UCMGInteractInterface>())
			{
				return;
			}

			if (!ICMGInteractInterface::Execute_CanInteract(InteractableActor))
			{
				return;
			}

			constexpr bool bIsFocused = true;
			ICMGInteractInterface::Execute_UpdateFocus(InteractableActor, bIsFocused);
			FocusedActor = InteractableActor;
		}
		else if (FocusedActor.IsValid())
		{
			constexpr bool bIsFocused = false;
			ICMGInteractInterface::Execute_UpdateFocus(FocusedActor.Get(), bIsFocused);
			FocusedActor = nullptr;
		}
	}
	else if (FocusedActor.IsValid())
	{
		constexpr bool bIsFocused = false;
		ICMGInteractInterface::Execute_UpdateFocus(FocusedActor.Get(), bIsFocused);
		FocusedActor = nullptr;
	}
}
