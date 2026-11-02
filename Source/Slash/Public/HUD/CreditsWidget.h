// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditsWidget.generated.h"

UCLASS()
class SLASH_API UCreditsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void PlayCreditsSequence();

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	class USoundBase* CreditsMusic;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* CreditsRollAnimation;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void OnCreditsFinished();

private:

	bool bIsHoldingSkip = false;
	float CurrentHoldTime = 0.f;
	const float RequiredHoldTime = 2.0f;
	bool bHasQuit = false;

	bool bCanSkip = false;
	FTimerHandle SkipDelayTimer;
	void EnableSkipping();
};
