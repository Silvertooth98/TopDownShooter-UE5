// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/CheatManager.h>

#include "TDSCheatManager.generated.h"

/**
 * https://benui.ca/unreal/cheatmanager/
 */
UCLASS()
class TDS_API UTDSCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(exec)
	void ToggleInfiniteAmmo();
};
