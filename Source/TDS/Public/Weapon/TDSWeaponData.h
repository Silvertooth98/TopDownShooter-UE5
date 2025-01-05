// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>

#include "Weapon/TDSWeaponTypes.h"

#include "TDSWeaponData.generated.h"

class UAnimInstance;
class UAnimMontage;
class ATDSPickuppable;
class ATDSProjectile;

UCLASS(Abstract, Blueprintable)
class TDS_API UTDSWeaponData : public UPrimaryDataAsset
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
	FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSubclassOf<UAnimInstance> WeaponAnimInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float Damage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	float AttackDelay = 0.f;
};

UCLASS()
class TDS_API UTDSWeaponData_Ranged : public UTDSWeaponData
{
	GENERATED_BODY()

public:
	UTDSWeaponData_Ranged();

	// Begin UObject override
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UObject override

private:
	void SetupDefaultFireModesList(TMap<ETDSWeaponFireMode, bool>& Out_FireModesList);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> CharacterFireAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> CharacterReloadAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> CharacterDryFireAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	FName MuzzleName;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Weapon|Ranged|Projectile",
		Meta = (EditCondition = "UseMode == ETDSWeaponUseMode::Projectile", EditConditionHides))
	TSubclassOf<ATDSProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	float Range = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	int32 MaxAmmo = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	ETDSWeaponFireMode FireMode = ETDSWeaponFireMode::SemiAutomatic;
	UPROPERTY()
	ETDSWeaponFireMode PrevFireMode = ETDSWeaponFireMode::SemiAutomatic;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Weapon|Ranged",
		Meta = (
			EditCondition = "FireMode == ETDSWeaponFireMode::Burst", EditConditionHides,
			ClampMin = 2, UIMin = 2))
	int32 BulletsPerBurst = 2;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Weapon|Ranged",
		Meta = (
			EditCondition = "FireMode == ETDSWeaponFireMode::Burst", EditConditionHides,
			ClampMin = 0.05f, UIMin = 0.05f))
	float BurstDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	TMap<ETDSWeaponFireMode, bool> AllowedFireModesMap;
	UPROPERTY()
	TSet<ETDSWeaponFireMode> AllowedFireModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	TObjectPtr<UCurveVector> RecoilPattern;
};
