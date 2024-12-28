// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/TDSWeapon_Ranged.h"

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/CapsuleComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Engine/World.h>
#include <GameFramework/Character.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/KismetMathLibrary.h>

#include "Weapon/Projectile/TDSProjectile.h"

void ATDSWeapon_Ranged::BeginPlay()
{
	Super::BeginPlay();

	WeaponDataAsset_Ranged = Cast<UTDSWeaponData_Ranged>(WeaponDataAsset);
	if (WeaponDataAsset_Ranged == nullptr)
	{
		return;
	}
	CurrentAmmo = WeaponDataAsset_Ranged->MaxAmmo;
}

void ATDSWeapon_Ranged::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WeaponState == ETDSWeaponState::Active)
	{
		TimeSinceLastFired += DeltaSeconds;
		if (GetAttackDelay() <= TimeSinceLastFired)
		{
			TimeSinceLastFired = 0.f;
			WeaponState = ETDSWeaponState::Ready;
			if (!bContinueFiring)
			{
				return;
			}

			switch (GetFireMode())
			{
			case ETDSWeaponFireMode::Burst:
				// conor.micallefgreen 28/12/24 TODO: Check if burst is complete before firing again
				return;

			case ETDSWeaponFireMode::Automatic:
				StartShooting();
				break;

			case ETDSWeaponFireMode::Single:		// Intentional fall-through
			case ETDSWeaponFireMode::SemiAutomatic:	// Intentional fall-through
			default:
				break;
			}
		}
	}
}

void ATDSWeapon_Ranged::StartShooting()
{
	// When the player stops shooting and tries to shoot again whilst the FireDelay is active, set bContinueFiring
	//	to true, so once the FireDelay ends, the next shot will trigger if possible.
	if (WeaponState == ETDSWeaponState::Active && !bContinueFiring)
	{
		bContinueFiring = true;
		return;
	}

	if (!CanShoot())
	{
		return;
	}

	if (!HasAmmo())
	{
		PlayDryFireMontage();
		return;
	}

	switch (GetWeaponUseMode())
	{
	case ETDSWeaponUseMode::HitScan:
		Shoot_HitScan();
		break;

	case ETDSWeaponUseMode::Projectile:
		Shoot_Projectile();
		break;

	case ETDSWeaponUseMode::Melee:	// Intentional fall-through
	default:
		break;
	}
}

void ATDSWeapon_Ranged::StopShooting()
{
	bContinueFiring = false;
}

void ATDSWeapon_Ranged::Reload()
{
	if (!CanReload())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	UAnimMontage* ReloadAnimMontageToPlay = GetCharacterReloadAnimMontage();
	if (PlayMontage(ReloadAnimMontageToPlay, 1.f, NAME_None, 0.f) > 0.f)
	{
		WeaponState = ETDSWeaponState::Reloading;

		OnMontageEndedDelegate.BindUObject(this, &ThisClass::OnReloadMontageEnded);
		AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, ReloadAnimMontageToPlay);
	}
}

void ATDSWeapon_Ranged::Shoot_HitScan()
{
	const UWorld* const World = GetWorld();
	if (World == nullptr || WeaponSkeletalMesh == nullptr)
	{
		return;
	}
	
	const FVector StartLoc = WeaponSkeletalMesh->GetSocketLocation(GetMuzzleName());
	const FVector TargetLoc = CalculateTargetLocation();

	FHitResult HitResult;
	// conor.micallefgreen 28/12/24 TODO: Check collision channel
	World->LineTraceSingleByChannel(HitResult, StartLoc, TargetLoc, ECollisionChannel::ECC_Pawn);
	
	BP_OnHitScanBulletFired(HitResult.Location, HitResult.GetActor(), HitResult.GetComponent());

	BulletFired();
}

void ATDSWeapon_Ranged::Shoot_Projectile()
{
	UWorld* const World = GetWorld();
	if (World == nullptr || WeaponSkeletalMesh == nullptr)
	{
		return;
	}

	const FTransform& MuzzleSocketTransform = WeaponSkeletalMesh->GetSocketTransform(GetMuzzleName());
	const FVector StartLoc = MuzzleSocketTransform.GetLocation();
	const FVector TargetLoc = CalculateTargetLocation();

	//const FVector TargetLoc = StartLoc + MuzzleSocketTransform.RotateVector(MuzzleOffset);
	const FRotator Rot =
		UKismetMathLibrary::FindLookAtRotation(StartLoc, TargetLoc);
	const FVector ScaleVec = FVector(1.f);

	FTransform ProjectileSpawnTransform = FTransform(Rot, StartLoc, ScaleVec);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	AActor* SpawnedProjectile =
		World->SpawnActor(GetProjectileClass(), &ProjectileSpawnTransform, SpawnParams);

	BulletFired();

	if (SpawnedProjectile == nullptr)
	{
		return;
	}

	// Set the spawned projectile to be ignored by the owning character and this weapon, so the projectile doesn't
	//	collide with either
	const bool bShouldIgnoreProjectile = true;
	if (ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent())
		{
			CapsuleComp->IgnoreActorWhenMoving(SpawnedProjectile, bShouldIgnoreProjectile);
		}
	}

	WeaponSkeletalMesh->IgnoreActorWhenMoving(SpawnedProjectile, bShouldIgnoreProjectile);
}

void ATDSWeapon_Ranged::BulletFired()
{
	CurrentAmmo--;
	WeaponState = ETDSWeaponState::Active;
	
	bContinueFiring = true;
	TimeSinceLastFired = 0.f;

	BP_OnBulletFired();

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	UAnimMontage* AnimMontageToPlay = GetCharacterFireAnimMontage();
	PlayMontage(AnimMontageToPlay, 1.f, NAME_None, 0.f);
}

FVector ATDSWeapon_Ranged::CalculateTargetLocation() const
{
	FVector TargetLoc;
	ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
	{
		return TargetLoc;
	}
	
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (PC == nullptr)
	{
		return TargetLoc;
	}

	const FVector& MuzzleLoc = WeaponSkeletalMesh->GetSocketLocation(GetMuzzleName());

	FHitResult HitRes;
	PC->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitRes);

	FVector CursorLoc = FVector(HitRes.Location.X, HitRes.Location.Y, MuzzleLoc.Z);

	// * 100 to get range in cm
	TargetLoc = CursorLoc + (OwnerCharacter->GetActorForwardVector() * (GetRange() * 100));
	return TargetLoc;
}

void ATDSWeapon_Ranged::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		if (WeaponState == ETDSWeaponState::Reloading)
		{
			WeaponState = ETDSWeaponState::Ready;
		}
		return;
	}
	WeaponState = ETDSWeaponState::Ready;
	CurrentAmmo = GetMaxAmmo();
}

void ATDSWeapon_Ranged::PlayDryFireMontage()
{
	check(!HasAmmo());
	WeaponState = ETDSWeaponState::Ready;

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	UAnimMontage* AnimMontageToPlay = GetCharacterDryFireAnimMontage();
	PlayMontage(AnimMontageToPlay, 1.f, NAME_None, 0.f);
}

float ATDSWeapon_Ranged::PlayMontage(
	UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	float Duration = -1.f;
	if (NewAnimMontage == nullptr)
	{
		return Duration;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance == nullptr)
	{
		return Duration;
	}

	Duration = AnimInstance->Montage_Play(
			NewAnimMontage, 1.f, EMontagePlayReturnType::MontageLength, /*StartTimeSeconds*/ 0.f);
	if (Duration > 0.f)
	{
		// conor.micallefgreen 23/12/24 TODO: Store NewAnimMontage in property inside TDSWeaponBase
	}
	return Duration;
}

UAnimInstance* ATDSWeapon_Ranged::GetOwnerAnimInstance() const
{
	ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
	{
		return nullptr;
	}
	return OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
}
