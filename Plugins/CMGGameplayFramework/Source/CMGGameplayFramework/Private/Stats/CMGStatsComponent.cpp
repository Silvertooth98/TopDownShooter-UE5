// Fill out your copyright notice in the Description page of Project Settings.

#include "Stats/CMGStatsComponent.h"

UCMGStatsComponent::UCMGStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame. You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCMGStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	RuntimeStats = DefaultStats;
	for (FCMGStatData& RuntimeStat : RuntimeStats)
	{
		RuntimeStat.CurrentValue = RuntimeStat.MaxValue;
	}
}

void UCMGStatsComponent::UpdateStat(const FGameplayTag& StatTag, float Delta)
{
	FCMGStatData& Stat = GetStatData(StatTag);
	Stat.CurrentValue += Delta;
}

FCMGStatData& UCMGStatsComponent::GetStatData(const FGameplayTag& StatTag)
{
	static FCMGStatData InvalidStat;
	if (!StatTag.IsValid())
	{
		return InvalidStat;
	}
	return *RuntimeStats.FindByPredicate([StatTag](const FCMGStatData& Elem)
		{
			return Elem.Tag == StatTag;
		});
}
