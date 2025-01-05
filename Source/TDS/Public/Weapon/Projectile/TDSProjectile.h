// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Actor.h>

#include <ObjectManagement/CMGPoolableObject.h>

#include "TDSProjectile.generated.h"

class AActor;
class UBoxComponent;
class UPrimitiveComponent;
class UProjectileMovementComponent;
class URadialForceComponent;
class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable)
class TDS_API ATDSProjectile : public AActor, public ICMGPoolableObject
{
	GENERATED_BODY()
	
public:	
	ATDSProjectile();

	// Begin UObject override
	void BeginPlay() override;
	// End UObject override

	// Begin ICMGPoolableObject override
	void SetLastUsedTime_Implementation(float Time) override;
	float GetLastUsedTime_Implementation() const override;
	// End ICMGPoolableObject override

	void InitializeProjectile(const FVector& Direction);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void BP_OnProjectileHit(FVector HitLocation);

	UFUNCTION()
	void OnComponentHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector normalImpulse,
		const FHitResult& Hit);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URadialForceComponent> RadialForceComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	float LastUsedTime = 0.f;
};

inline void ATDSProjectile::SetLastUsedTime_Implementation(float Time)
{
	LastUsedTime = Time;
}

inline float ATDSProjectile::GetLastUsedTime_Implementation() const
{
	return LastUsedTime;
}
