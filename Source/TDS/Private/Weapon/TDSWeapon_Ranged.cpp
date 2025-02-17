// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/TDSWeapon_Ranged.h"

#include <Animation/AnimMontage.h>
#include <Components/CapsuleComponent.h>
#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <GameFramework/Character.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/KismetMathLibrary.h>
#include <TimerManager.h>

#include <ObjectManagement/CMGWorldObjectPoolSubsystem.h>
#include <Weapon/Projectile/CMGProjectile.h>

void ATDSWeapon_Ranged::BeginPlay()
{
	Super::BeginPlay();

	WeaponDataAsset_Ranged = Cast<UTDSWeaponData_Ranged>(WeaponDataAsset);
	if (WeaponDataAsset_Ranged == nullptr)
	{
		return;
	}
	CurrentAmmo = WeaponDataAsset_Ranged->MaxAmmo;
	CurrentFireMode = WeaponDataAsset_Ranged->FireMode;
}

void ATDSWeapon_Ranged::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WeaponState == ETDSWeaponState::Active)
	{
		TimeSinceLastFired += DeltaSeconds;
	}
}

void ATDSWeapon_Ranged::StartShooting()
{
	// When the player stops holding the shoot input and tries to shoot again whilst the weapon is still
	//	actively shooting, set bStopShooting back to false, so the next shot will trigger when required
	if (WeaponState == ETDSWeaponState::Active && bStopShooting)
	{
		bStopShooting = false;
		return;
	}

	if (!CanShoot())
	{
		return;
	}

	ShootWeapon();
}

void ATDSWeapon_Ranged::StopShooting()
{
	if (WeaponState != ETDSWeaponState::Active)
	{
		return;
	}

	ShootHandle.Invalidate();
	bStopShooting = true;
}

void ATDSWeapon_Ranged::Reload()
{
	if (!CanReload())
	{
		return;
	}

	UAnimMontage* ReloadAnimMontageToPlay = GetCharacterReloadAnimMontage();
	TFunction<void(UAnimMontage*, bool)> OnReloadMontageEndedCallback =
		[this](UAnimMontage* Montage, bool bInterrupted)
		{
			OnMontageEnded(Montage, bInterrupted);

			if (bInterrupted)
			{
				return;
			}
			WeaponState = ETDSWeaponState::Ready;
			CurrentAmmo = GetMaxAmmo();
			BP_OnReload();
		};
	if (PlayMontage(ReloadAnimMontageToPlay, 1.f, NAME_None, 0.f, OnReloadMontageEndedCallback) > 0.f)
	{
		WeaponState = ETDSWeaponState::Reloading;
	}
}

void ATDSWeapon_Ranged::ToggleFireMode()
{
	const TSet<ETDSWeaponFireMode> AllowedFireModes = GetAllowedFireModes();
	if (AllowedFireModes.Num() == 1)
	{
		return;
	}

	ETDSWeaponFireMode NextFireMode = CurrentFireMode;
	uint8 EnumMaxByte = static_cast<uint8>(ETDSWeaponFireMode::Max);
	uint8 CurrentValueByte = static_cast<uint8>(CurrentFireMode);

	bool bIsNextFireModeAllowed = false;
	while (!bIsNextFireModeAllowed)
	{
		CurrentValueByte = (CurrentValueByte + 1) % EnumMaxByte;
		NextFireMode = static_cast<ETDSWeaponFireMode>(CurrentValueByte);
		bIsNextFireModeAllowed =
			AllowedFireModes.Contains(NextFireMode) || NextFireMode == CurrentFireMode;
	}
	CurrentFireMode = NextFireMode;
}

void ATDSWeapon_Ranged::ShootWeapon()
{
	// If player is no longer holding the shoot button, or the burst has finished, stop shooting
	if (bStopShooting)
	{
		const ETDSWeaponFireMode FireMode = GetFireMode();
		// For burst weapons, don't block the next bullet being fired when requested until all required
		//	bullets in the burst have been fired
		if (FireMode != ETDSWeaponFireMode::Burst ||
			FireMode == ETDSWeaponFireMode::Burst && BulletsFiredInBurst == 0)
		{
			bStopShooting = false;
			ShootHandle.Invalidate();
			WeaponState = ETDSWeaponState::Ready;
			return;
		}
	}
	// Stop single-shot and semi-automatic weapons from firing again.
	// Need to do this here via the timer set within BulletFired() to block the player from being
	//	able to fire faster than the weapon's AttackDelay
	else
	{
		switch (GetFireMode())
		{
		case ETDSWeaponFireMode::SemiAutomatic:
		case ETDSWeaponFireMode::Single:
			if (WeaponState == ETDSWeaponState::Active)
			{
				ShootHandle.Invalidate();
				WeaponState = ETDSWeaponState::Ready;
				return;
			}
			break;

		default:
			break;
		}
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

	UCMGWorldObjectPoolSubsystem* ObjectPoolWorldSubsystem = World->GetSubsystem<UCMGWorldObjectPoolSubsystem>();
	if (ObjectPoolWorldSubsystem == nullptr)
	{
		return;
	}

	const FTransform& MuzzleSocketTransform = WeaponSkeletalMesh->GetSocketTransform(GetMuzzleName());
	const FVector StartLoc = MuzzleSocketTransform.GetLocation();
	const FVector TargetLoc = CalculateTargetLocation();

	// conor.micallefgreen 31/12/24 TODO: Implement MuzzleOffset value
	//const FVector TargetLoc = StartLoc + MuzzleSocketTransform.RotateVector(MuzzleOffset); 
	const FRotator Rot =
		UKismetMathLibrary::FindLookAtRotation(StartLoc, TargetLoc);
	const FVector ScaleVec = FVector(1.f);

	FTransform ProjectileSpawnTransform = FTransform(Rot, StartLoc, ScaleVec);

	ACMGProjectile* Projectile = ObjectPoolWorldSubsystem->GetOrCreateObject(GetProjectileClass());
	if (Projectile == nullptr)
	{
		return;
	}

	FCMGProjectileData ProjectileData(this, Cast<APawn>(GetOwner()), (TargetLoc - StartLoc).GetSafeNormal());
	Projectile->InitializeProjectile(ProjectileSpawnTransform, ProjectileData);

	BulletFired();

	// Set the spawned projectile to be ignored by the owning character and this weapon, so the projectile doesn't
	//	collide with either
	const bool bShouldIgnoreProjectile = true;
	if (ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent())
		{
			CapsuleComp->IgnoreActorWhenMoving(Projectile, bShouldIgnoreProjectile);
		}
	}

	WeaponSkeletalMesh->IgnoreActorWhenMoving(Projectile, bShouldIgnoreProjectile);
}

void ATDSWeapon_Ranged::BulletFired()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	if (!CVarInfiniteAmmo.GetValueOnGameThread())
	{
		CurrentAmmo--;
	}
#else
	CurrentAmmo--;
#endif	// !UE_BUILD_SHIPPING

	WeaponState = ETDSWeaponState::Active;
	
	TimeSinceLastFired = 0.f;

	BP_OnBulletFired();

	switch (GetFireMode())
	{
	case ETDSWeaponFireMode::Automatic:		// Intentional fall-through
	case ETDSWeaponFireMode::SemiAutomatic:	// Intentional fall-through
	case ETDSWeaponFireMode::Single:
		World->GetTimerManager().SetTimer(
			ShootHandle,
			this,
			&ThisClass::ShootWeapon,
			GetAttackDelay(),
			/*bLooping*/ false);
		break;
	case ETDSWeaponFireMode::Burst:
	{
		BulletsFiredInBurst++;
		float DelayTime = GetAttackDelay();
		// If the burst has fired all of the required bullets, delay the next shot by the BurstDelay
		if (BulletsFiredInBurst == GetBulletsPerBurst())
		{
			DelayTime = GetBurstDelay();
			BulletsFiredInBurst = 0;
		}
		World->GetTimerManager().SetTimer(
			ShootHandle,
			this,
			&ThisClass::ShootWeapon,
			DelayTime,
			/*bLooping*/ false);
	}
	break;

	default:
		break;
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

void ATDSWeapon_Ranged::PlayDryFireMontage()
{
	check(!HasAmmo());
	WeaponState = ETDSWeaponState::Ready;

	UAnimMontage* AnimMontageToPlay = GetCharacterDryFireAnimMontage();
	PlayMontage(AnimMontageToPlay, 1.f, NAME_None, 0.f);
}
