// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Actor.h>

#include "TDSWeaponBase.generated.h"

class UAnimInstance;
class UAnimMontage;
class ATDSPickuppable;
class UCurveVector;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class ETDSWeaponUseMode : uint8
{
	HitScan,
	Projectile,
	Melee
};

UENUM(BlueprintType)
enum class ETDSWeaponType : uint8
{
	Primary,
	Secondary,
	Utility
};

// Move into separate structs for melee and ranged weapons
USTRUCT(BlueprintType)
struct FTDSWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	FName WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	ETDSWeaponType WeaponType = ETDSWeaponType::Primary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	ETDSWeaponUseMode UseMode = ETDSWeaponUseMode::Projectile;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	TSubclassOf<ATDSPickuppable> PickupClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	TSubclassOf<UAnimInstance> WeaponAnimInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	TObjectPtr<UAnimMontage> CharacterFireAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float Damage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float Range = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float MaxAmmo = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float FireDelay = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	TObjectPtr<UCurveVector> RecoilPattern;
};

UCLASS(Abstract, Blueprintable)
class TDS_API ATDSWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATDSWeaponBase();

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	void Equip();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data", Meta = (AllowPrivateAccess = "true"))
	FTDSWeaponData WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;
};
