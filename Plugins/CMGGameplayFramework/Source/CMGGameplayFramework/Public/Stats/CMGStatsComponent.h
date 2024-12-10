// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>
#include <GameplayTagContainer.h>

#include "CMGStatsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCMGOnStatUpdated, FCMGStatData /* StatUpdated */);

USTRUCT(BlueprintType)
struct FCMGStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
	float MaxValue = 0;

	float CurrentValue = 0;

	// Used for triggering specific logic when specific stats update
	FCMGOnStatUpdated OnStatUpdated;
};

UCLASS(Blueprintable)
class CMGGAMEPLAYFRAMEWORK_API UCMGStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCMGStatsComponent();

	// Begin UActorComponent Overrides
	void BeginPlay() override;
	// End UActorComponent Overrides

	void UpdateStat(const FGameplayTag& StatTag, float Delta);

	FCMGStatData& GetStatData(const FGameplayTag& StatTag);

public:
	// Broadcasted whenever any stat changes regardless of the stat itself
	FCMGOnStatUpdated OnAnyStatUpdated;

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<FCMGStatData> DefaultStats;

	TArray<FCMGStatData> RuntimeStats;
};
