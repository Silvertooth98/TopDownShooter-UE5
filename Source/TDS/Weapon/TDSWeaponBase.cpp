// Fill out your copyright notice in the Description page of Project Settings.

#include "TDSWeaponBase.h"

#include <Components/SkeletalMeshComponent.h>

ATDSWeaponBase::ATDSWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	check(WeaponSkeletalMesh);
	SetRootComponent(WeaponSkeletalMesh);
	WeaponSkeletalMesh->SetComponentTickEnabled(false);
}

void ATDSWeaponBase::Equip()
{
}

// Called when the game starts or when spawned
void ATDSWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATDSWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
