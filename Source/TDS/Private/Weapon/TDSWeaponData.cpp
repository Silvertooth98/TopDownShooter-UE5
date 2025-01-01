// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/TDSWeaponData.h"

UTDSWeaponData_Ranged::UTDSWeaponData_Ranged()
{
	PrevFireMode = FireMode;
	SetupDefaultFireModesList(AllowedFireModesMap);
}

void UTDSWeaponData_Ranged::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// If there default FireMode was changed, remove the previous entry from the list of AllowedFireModesMap
	//	and add the newly selected default FireMode
	const FName PropertyName =
		PropertyChangedEvent.Property != nullptr
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, FireMode))
	{
		if (bool* PrevFireModePtr = AllowedFireModesMap.Find(PrevFireMode))
		{
			*PrevFireModePtr = false;
		}
		PrevFireMode = FireMode;
		if (bool* NewFireModePtr = AllowedFireModesMap.Find(FireMode))
		{
			*NewFireModePtr = true;
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, AllowedFireModesMap))
	{
		// Ensure no enum types are added or removed
		TArray<ETDSWeaponFireMode> Keys;
		AllowedFireModesMap.GetKeys(Keys);

		// Create default mapping with all available types
		TMap<ETDSWeaponFireMode, bool> DefaultFireModes;
		SetupDefaultFireModesList(DefaultFireModes);

		AllowedFireModes.Empty(DefaultFireModes.Num());

		// Restore original map values with existing data
		for (auto& Elem : DefaultFireModes)
		{
			const ETDSWeaponFireMode& Key = Elem.Key;
			if (FireMode == Key)
			{
				Elem.Value = true;
			}
			else if (AllowedFireModesMap.Contains(Key))
			{
				Elem.Value = AllowedFireModesMap[Key];
			}
			if (Elem.Value)
			{
				AllowedFireModes.Add(Elem.Key);
			}
		}

		AllowedFireModesMap = DefaultFireModes;
	}
}

void UTDSWeaponData_Ranged::SetupDefaultFireModesList(
	TMap<ETDSWeaponFireMode, bool>& Out_FireModesList)
{
	Out_FireModesList.Add(ETDSWeaponFireMode::Single, false);
	Out_FireModesList.Add(ETDSWeaponFireMode::SemiAutomatic, true);
	Out_FireModesList.Add(ETDSWeaponFireMode::Burst, false);
	Out_FireModesList.Add(ETDSWeaponFireMode::Automatic, false);
}
