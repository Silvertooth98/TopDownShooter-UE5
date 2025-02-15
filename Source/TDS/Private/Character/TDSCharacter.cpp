// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/TDSCharacter.h"

#include <Camera/CameraComponent.h>
#include <Components/DecalComponent.h>
#include <Components/CapsuleComponent.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Materials/Material.h>
#include <UObject/ConstructorHelpers.h>

#include <CMGFramework.h>

ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Don't rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LookAt(DeltaSeconds);
}

void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	}
}

void ATDSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();

	const FVector RightVector = UKismetMathLibrary::GetRightVector(
		FRotator(ControlRotation.Roll, 0.0f, ControlRotation.Yaw));

	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(
		FRotator(0.f, 0.f, ControlRotation.Yaw));

	AddMovementInput(ForwardVector, MovementVector.Y);
	AddMovementInput(RightVector, MovementVector.X);
}

void ATDSCharacter::LookAt(float DeltaSeconds)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	if (!PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult))
	{
		return;
	}

	const FRotator ActorRot = GetActorRotation();
	const FVector Target = HitResult.ImpactPoint - GetActorLocation();
	LookAtTarget = UKismetMathLibrary::FindLookAtRotation(FVector(0.f), Target);
	const FRotator InterpTo = UKismetMathLibrary::RInterpTo_Constant(ActorRot, LookAtTarget, DeltaSeconds, RotationSpeed);
	SetActorRotation(FRotator(ActorRot.Pitch, InterpTo.Yaw, ActorRot.Roll));
}
