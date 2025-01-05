// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Actor.h>
#include <Templates/Function.h>

#include "Weapon/TDSWeaponTypes.h"
#include "Weapon/TDSWeaponData.h"

#include "TDSWeaponBase.generated.h"

class UAnimInstance;
class UAnimMontage;
class UCurveVector;
class USkeletalMeshComponent;

class ATDSPickuppable;

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<bool> CVarInfiniteAmmo(
	TEXT("InfiniteAmmo"),
	false,
	TEXT("Enable/Disable Infinite Ammo Cheat. true = Infinite Ammo, false = Normal Ammo"),
	ECVF_Cheat);
#endif	// !UE_BUILD_SHIPPING

UCLASS(Abstract, Blueprintable)
class TDS_API ATDSWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATDSWeaponBase();

	// Begin UObject override
	void BeginPlay() override;
	// End UObject override

	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	virtual void Equip() {}
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	virtual void Unequip() {}
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	virtual void Drop();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	FName GetWeaponData() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	ETDSWeaponType GetWeaponType() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	ETDSWeaponUseMode GetWeaponUseMode() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	TSubclassOf<ATDSPickuppable> GetPickupClass() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	FName GetSocketName() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	FTransform GetAttachTransform() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	TSubclassOf<UAnimInstance> GetWeaponAnimInstance() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetDamage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetAttackDelay() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	virtual bool IsWeaponIsUse() const;

protected:
	float PlayMontage(
		UAnimMontage* NewAnimMontage,
		float InPlayRate,
		FName StartSectionName,
		float StartTimeSeconds);
	float PlayMontage(
		UAnimMontage* NewAnimMontage,
		float InPlayRate,
		FName StartSectionName,
		float StartTimeSeconds,
		TFunction<void(UAnimMontage*, bool)> CallbackFunc);

	UAnimInstance* GetOwnerAnimInstance() const;

	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTDSWeaponData> WeaponDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;

	UPROPERTY(Transient)
	TWeakObjectPtr<UAnimInstance> OwnerAnimInstance;

	UPROPERTY(Transient)
	ETDSWeaponState WeaponState = ETDSWeaponState::Ready;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> ActiveAnimMontage = nullptr;

	FOnMontageEnded OnMontageEndedDelegate;
};

inline FName ATDSWeaponBase::GetWeaponData() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->WeaponName : TEXT("Invalid");
}

inline ETDSWeaponType ATDSWeaponBase::GetWeaponType() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->WeaponType : ETDSWeaponType::Primary;
}

inline ETDSWeaponUseMode ATDSWeaponBase::GetWeaponUseMode() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->UseMode : ETDSWeaponUseMode::HitScan;
}

inline TSubclassOf<ATDSPickuppable> ATDSWeaponBase::GetPickupClass() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->PickupClass : nullptr;
}

inline FName ATDSWeaponBase::GetSocketName() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->SocketName : TEXT("Invalid");
}

inline FTransform ATDSWeaponBase::GetAttachTransform() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->AttachTransform : FTransform();
}

inline TSubclassOf<UAnimInstance> ATDSWeaponBase::GetWeaponAnimInstance() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->WeaponAnimInstance : nullptr;
}

inline float ATDSWeaponBase::GetDamage() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->Damage : 0.f;
}

inline float ATDSWeaponBase::GetAttackDelay() const
{
	return WeaponDataAsset != nullptr ? WeaponDataAsset->AttackDelay : 0.f;
}

inline bool ATDSWeaponBase::IsWeaponIsUse() const
{
	return WeaponState == ETDSWeaponState::Active;
}

inline UAnimInstance* ATDSWeaponBase::GetOwnerAnimInstance() const
{
	return OwnerAnimInstance.Get();
}
