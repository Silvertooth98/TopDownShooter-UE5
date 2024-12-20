// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSPlayerController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATDSPlayerController::ATDSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}
