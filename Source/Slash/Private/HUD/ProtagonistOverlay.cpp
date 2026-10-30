// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ProtagonistOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UProtagonistOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UProtagonistOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UProtagonistOverlay::SetGold(int32 Gold)
{
	if (GoldCount)
	{
		const FString GoldString = FString::Printf(TEXT("%d"), Gold);
		const FText GoldText = FText::FromString(GoldString);
		GoldCount->SetText(GoldText);
	}
}

void UProtagonistOverlay::SetSouls(int32 Soul)
{
	if (SoulCount)
	{
		const FString SoulString = FString::Printf(TEXT("%d"), Soul);
		const FText SoulText = FText::FromString(SoulString);
		SoulCount->SetText(SoulText);
	}
}

void UProtagonistOverlay::SetHealthPotions(int32 HealthPotion)
{
	if (HealthPotionCount)
	{
		const FString HPString = FString::Printf(TEXT("%d"), HealthPotion);
		const FText HPText = FText::FromString(HPString);
		HealthPotionCount->SetText(HPText);
	}
}
