// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/TDSProjectile.h"

#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <PhysicsEngine/RadialForceComponent.h>

ATDSProjectile::ATDSProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	check(BoxComponent);
	SetRootComponent(BoxComponent);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	check(ProjectileMesh);
	ProjectileMesh->SetupAttachment(BoxComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	check(RadialForceComponent);
	RadialForceComponent->SetupAttachment(ProjectileMesh);

	ProjectileMovementComponent =
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	check(ProjectileMovementComponent);
}

void ATDSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	const bool bShouldIgnore = true;
	BoxComponent->IgnoreActorWhenMoving(GetOwner(), bShouldIgnore);
	BoxComponent->IgnoreActorWhenMoving(GetInstigator(), bShouldIgnore);

	BoxComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnComponentHit);
}

void ATDSProjectile::OnComponentHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector normalImpulse,
	const FHitResult& Hit)
{
	RadialForceComponent->FireImpulse();
	BP_OnProjectileHit(Hit.Location);
	Destroy();
}
