// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProtagonistOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UProtagonistOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Soul);
	void SetHealthPotions(int32 HealthPotion);

private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthPotionCount;
};
