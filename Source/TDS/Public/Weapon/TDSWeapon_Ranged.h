// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include "Weapon/TDSWeaponBase.h"
#include "Weapon/TDSWeaponTypes.h"

#include "TDSWeapon_Ranged.generated.h"

class AActor;
class UAnimInstance;
class UAnimMontage;
class UPrimitiveComponent;

class ATDSProjectile;

struct FTimerHandle;

/**
 * Base class for ranged weapons
 */
UCLASS()
class TDS_API ATDSWeapon_Ranged : public ATDSWeaponBase
{
	GENERATED_BODY()
	
public:
	// Begin UObject override
	void BeginPlay() override;
	// End UObject override

	// Begin AActor override
	void Tick(float DeltaSeconds) override;
	// End AActor override

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartShooting();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopShooting();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ToggleFireMode();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	UAnimMontage* GetCharacterFireAnimMontage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	UAnimMontage* GetCharacterReloadAnimMontage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	UAnimMontage* GetCharacterDryFireAnimMontage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	FName GetMuzzleName() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	TSubclassOf<ATDSProjectile> GetProjectileClass() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetRange() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetMaxAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetCurrentAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	ETDSWeaponFireMode GetFireMode() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	ETDSWeaponFireMode GetDefaultFireMode() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	TSet<ETDSWeaponFireMode> GetAllowedFireModes() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetBulletsPerBurst() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetBurstDelay() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnBulletFired();
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnHitScanBulletFired(
		FVector HitResultLocation, AActor* HitActor, UPrimitiveComponent* HitComponent);

	// conor.micallefgreen 01/01/25 TODO: Move to delegate that the UI binds to update the ammo counter
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnReload();

private:
	UFUNCTION()
	void ShootWeapon();

	void Shoot_HitScan();
	void Shoot_Projectile();
	void BulletFired();

	FVector CalculateTargetLocation() const;

	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool CanShoot() const;
	bool HasAmmo() const;
	bool CanReload() const;

	void PlayDryFireMontage();

	float PlayMontage(
		UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds);

	UAnimInstance* GetOwnerAnimInstance() const;

protected:
	FOnMontageEnded OnMontageEndedDelegate;

	UPROPERTY(Transient)
	TObjectPtr<UTDSWeaponData_Ranged> WeaponDataAsset_Ranged;

	// conor.micallefgreen 22/12/24 TODO: Change to BlueprintReadOnly once weapon system is ported to code
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Weapon|Data")
	int32 CurrentAmmo = 0;

	bool bStopShooting = false;
	bool bBurstDelayActive = false;

	float TimeSinceLastFired = 0.f;
	int32 BulletsFiredInBurst = 0;

	FTimerHandle ShootHandle;
	ETDSWeaponFireMode CurrentFireMode = ETDSWeaponFireMode::SemiAutomatic;
};

inline UAnimMontage* ATDSWeapon_Ranged::GetCharacterFireAnimMontage() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->CharacterFireAnimMontage : nullptr;
}

inline UAnimMontage* ATDSWeapon_Ranged::GetCharacterReloadAnimMontage() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->CharacterReloadAnimMontage : nullptr;
}

inline UAnimMontage* ATDSWeapon_Ranged::GetCharacterDryFireAnimMontage() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->CharacterDryFireAnimMontage : nullptr;
}

inline FName ATDSWeapon_Ranged::GetMuzzleName() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->MuzzleName : TEXT("Invalid");
}

inline TSubclassOf<ATDSProjectile> ATDSWeapon_Ranged::GetProjectileClass() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->ProjectileClass : nullptr;
}

inline float ATDSWeapon_Ranged::GetRange() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->Range : 0.f;
}

inline int32 ATDSWeapon_Ranged::GetMaxAmmo() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->MaxAmmo : 0;
}

inline int32 ATDSWeapon_Ranged::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

inline ETDSWeaponFireMode ATDSWeapon_Ranged::GetFireMode() const
{
	return CurrentFireMode;
}

inline ETDSWeaponFireMode ATDSWeapon_Ranged::GetDefaultFireMode() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->FireMode : ETDSWeaponFireMode::SemiAutomatic;
}

inline TSet<ETDSWeaponFireMode> ATDSWeapon_Ranged::GetAllowedFireModes() const
{
	return WeaponDataAsset_Ranged != nullptr
		? WeaponDataAsset_Ranged->AllowedFireModes
		: TSet<ETDSWeaponFireMode>();
}

inline int32 ATDSWeapon_Ranged::GetBulletsPerBurst() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->BulletsPerBurst : 0;
}

inline float ATDSWeapon_Ranged::GetBurstDelay() const
{
	return WeaponDataAsset_Ranged != nullptr ? WeaponDataAsset_Ranged->BurstDelay : 0.f;
}

inline bool ATDSWeapon_Ranged::CanShoot() const
{
	return WeaponState == ETDSWeaponState::Ready
		|| (WeaponState == ETDSWeaponState::Reloading && HasAmmo());
}

inline bool ATDSWeapon_Ranged::HasAmmo() const
{
	return CurrentAmmo > 0;
}

inline bool ATDSWeapon_Ranged::CanReload() const
{
	return CurrentAmmo < GetMaxAmmo() && WeaponState == ETDSWeaponState::Ready;
}
