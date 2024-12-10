// Fill out your copyright notice in the Description page of Project Settings.

#include "Health/CMGHealthComponent.h"

#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>

UCMGHealthComponent::UCMGHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame. You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UCMGHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	Owner->OnTakeAnyDamage.AddDynamic(this, &ThisClass::TakeDamage);
}

void UCMGHealthComponent::Heal(float Amount)
{
	UpdateHealth(Amount);
}

void UCMGHealthComponent::DealDamage(float Amount)
{
	UpdateHealth(-Amount);
	if (CurrentHealth <= 0)
	{
		OnDeath.ExecuteIfBound();
	}
}

void UCMGHealthComponent::TakeDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	DealDamage(Damage);
}

void UCMGHealthComponent::SetHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(Amount, 0, MaxHealth);
}

void UCMGHealthComponent::UpdateHealth(float Update)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Update, 0, MaxHealth);
}
