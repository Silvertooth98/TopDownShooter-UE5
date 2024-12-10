// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheats/CMGCheatManager.h"

#include "Health/CMGHealthComponent.h"

void UCMGCheatManager::HealPlayer(float Amount)
{
	UCMGHealthComponent* HealthComp = GetHealthComponent();
	if (!HealthComp)
	{
		return;
	}
	HealthComp->Heal(Amount);
}

void UCMGCheatManager::DamagePlayer(float Amount)
{
	UCMGHealthComponent* HealthComp = GetHealthComponent();
	if (!HealthComp)
	{
		return;
	}
	HealthComp->DealDamage(Amount);
}

void UCMGCheatManager::SetHealth(float Amount)
{
	UCMGHealthComponent* HealthComp = GetHealthComponent();
	if (!HealthComp)
	{
		return;
	}
	HealthComp->SetHealth(Amount);
}

UCMGHealthComponent* UCMGCheatManager::GetHealthComponent() const
{
	APawn* Pawn = GetPlayerController()->GetPawn();
	return Pawn != nullptr ? Pawn->FindComponentByClass<UCMGHealthComponent>() : nullptr;
}
