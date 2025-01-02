// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/TDSWeaponBase.h"

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/Character.h>

ATDSWeaponBase::ATDSWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	check(WeaponSkeletalMesh);
	SetRootComponent(WeaponSkeletalMesh);
	WeaponSkeletalMesh->SetComponentTickEnabled(false);
}

void ATDSWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		OwnerAnimInstance =
			OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
	}
}

void ATDSWeaponBase::Drop()
{
	// Stop any active animation montage
	if (OwnerAnimInstance.IsValid() && ActiveAnimMontage != nullptr)
	{
		OwnerAnimInstance->Montage_Stop(
			ActiveAnimMontage->GetBlendOutArgs().BlendTime, ActiveAnimMontage);
		ActiveAnimMontage = nullptr;
	}

	// Unlink the weapon's anim instance
	if (TSubclassOf<UAnimInstance> WeaponAnimInstanceClass = GetWeaponAnimInstance())
	{
		if (ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
			{
				OwnerMesh->UnlinkAnimClassLayers(WeaponAnimInstanceClass);
			}
		}
	}

	Destroy();
}

float ATDSWeaponBase::PlayMontage(
	UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	TFunction<void(UAnimMontage*, bool)> OnMontageEndedCallback =
		[this](UAnimMontage* Montage, bool bInterrupted)
		{
			this->OnMontageEnded(Montage, bInterrupted);
		};
	return PlayMontage(
		NewAnimMontage,
		InPlayRate,
		StartSectionName,
		StartTimeSeconds,
		OnMontageEndedCallback);
}

float ATDSWeaponBase::PlayMontage(
	UAnimMontage* NewAnimMontage,
	float InPlayRate,
	FName StartSectionName,
	float StartTimeSeconds,
	TFunction<void(UAnimMontage*, bool)> CallbackFunc)
{
	float Duration = -1.f;
	if (!OwnerAnimInstance.IsValid() || NewAnimMontage == nullptr || CallbackFunc == nullptr)
	{
		return Duration;
	}

	// Stop the ActiveAnimMontage if it's still playing
	if (ActiveAnimMontage != nullptr && OwnerAnimInstance->Montage_IsPlaying(ActiveAnimMontage))
	{
		OwnerAnimInstance->Montage_Stop(
			ActiveAnimMontage->GetBlendOutArgs().BlendTime, ActiveAnimMontage);
	}

	Duration = OwnerAnimInstance->Montage_Play(
		NewAnimMontage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds);
	if (Duration > 0.f)
	{
		ActiveAnimMontage = NewAnimMontage;
		OnMontageEndedDelegate.BindLambda([CallbackFunc](UAnimMontage* Montage, bool bInterrupted)
			{
				CallbackFunc(Montage, bInterrupted);
			});
		OwnerAnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, NewAnimMontage);
	}
	return Duration;
}

void ATDSWeaponBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) 
	{
		return;
	}
	ActiveAnimMontage = nullptr;
}
