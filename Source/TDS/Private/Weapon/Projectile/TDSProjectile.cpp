// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Projectile/TDSProjectile.h"

#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <PhysicsEngine/RadialForceComponent.h>

#include <ObjectManagement/CMGWorldObjectPoolSubsystem.h>

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
	ProjectileMovementComponent->bAutoActivate = false;
}

void ATDSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	const bool bShouldIgnore = true;
	BoxComponent->IgnoreActorWhenMoving(GetOwner(), bShouldIgnore);
	BoxComponent->IgnoreActorWhenMoving(GetInstigator(), bShouldIgnore);

	BoxComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnComponentHit);
}

void ATDSProjectile::InitializeProjectile(const FVector& Direction)
{
	// Enable collision when the projectile is in use
	if (BoxComponent != nullptr)
	{
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (ProjectileMovementComponent != nullptr)
	{
		ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->GetMaxSpeed();
		ProjectileMovementComponent->Activate();
	}
}

void ATDSProjectile::OnComponentHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector normalImpulse,
	const FHitResult& Hit)
{
	const UWorld* const World = GetWorld();
	UCMGWorldObjectPoolSubsystem* ObjectPoolWorldSubsystem =
		World != nullptr ? World->GetSubsystem<UCMGWorldObjectPoolSubsystem>() : nullptr;
	if (ObjectPoolWorldSubsystem == nullptr)
	{
		return;
	}

	if (ProjectileMovementComponent != nullptr)
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->Velocity = FVector::ZeroVector;
		ProjectileMovementComponent->Deactivate();
	}

	RadialForceComponent->FireImpulse();
	BP_OnProjectileHit(Hit.Location);
	ObjectPoolWorldSubsystem->ReleasePoolObject(this);
}
