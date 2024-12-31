// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheats/TDSCheatManager.h"

#include "Weapon/TDSWeaponBase.h"

void UTDSCheatManager::ToggleInfiniteAmmo()
{
	CVarInfiniteAmmo->Set(!CVarInfiniteAmmo.GetValueOnGameThread());
}
