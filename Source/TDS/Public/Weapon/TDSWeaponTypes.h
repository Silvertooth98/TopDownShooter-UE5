// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TDSWeaponTypes.generated.h"

class UAnimInstance;
class UAnimMontage;
class ATDSPickuppable;
class UCurveVector;

UENUM(BlueprintType)
enum class ETDSWeaponUseMode : uint8
{
	HitScan,
	Projectile,
	Melee
};

UENUM(BlueprintType)
enum class ETDSWeaponType : uint8
{
	Primary,
	Secondary,
	Utility
};

UENUM(BlueprintType)
enum class ETDSWeaponFireMode : uint8
{
	Single			UMETA(ToolTip = "Single shot fired when triggered. Reload required after each shot (eg bolt-sniper)"),
	SemiAutomatic	UMETA(ToolTip = "Single shot fired when triggered. Automatic reload between shots (eg pistol)"),
	Burst			UMETA(ToolTip = "Burst weapon firing X number of shots when triggered"),
	Automatic		UMETA(ToolTip = "Fully automatic weapon, firing bullets whilst trigger is held down")
};

UENUM(BlueprintType)
enum class ETDSWeaponState : uint8
{
	Ready,
	Active,
	Reloading
};
