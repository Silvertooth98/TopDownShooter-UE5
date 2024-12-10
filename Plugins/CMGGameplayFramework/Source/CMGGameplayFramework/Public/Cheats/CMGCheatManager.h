// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/CheatManager.h>

#include "CMGCheatManager.generated.h"

class UCMGHealthComponent;

UCLASS()
class CMGGAMEPLAYFRAMEWORK_API UCMGCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(Exec)
	void HealPlayer(float Amount);

	UFUNCTION(Exec)
	void DamagePlayer(float Amount);

	UFUNCTION(Exec)
	void SetHealth(float Amount);

private:
	UCMGHealthComponent* GetHealthComponent() const;
};
