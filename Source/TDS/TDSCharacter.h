// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Character.h>
#include <InputActionValue.h>

#include "TDSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDSCharacter();

	// Called every frame.
	void Tick(float DeltaSeconds) override;

	// Begin APawn Overrides
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End APawn Overrides

	/** Returns TopDownCameraComponent subobject **/
	class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Inputs
	void Move(const FInputActionValue& Value);

	void LookAt(float DeltaSeconds);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 300.f;
};

