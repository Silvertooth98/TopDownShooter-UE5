// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/PlayerController.h>

#include "TDSPlayerController.generated.h"

class UNiagaraSystem;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class ATDSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATDSPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UNiagaraSystem> FXCursor;
};
