// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>

#include "CMGHealthComponent.generated.h"

class AActor;
class AController;
class UDamageType;

DECLARE_DELEGATE(FCMGOnHealthUpdated);

UCLASS(Blueprintable)
class CMGGAMEPLAYFRAMEWORK_API UCMGHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCMGHealthComponent();

	// Begin UActorComponent Overrides
	void BeginPlay() override;
	// End UActorComponent Overrides

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void DealDamage(float Amount);

	UFUNCTION()
	void TakeDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser);

	void SetHealth(float Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}

private:
	void UpdateHealth(float Update);

public:
	FCMGOnHealthUpdated OnDeath;

protected:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 0;

private:
	float CurrentHealth = FLT_MIN;
};
