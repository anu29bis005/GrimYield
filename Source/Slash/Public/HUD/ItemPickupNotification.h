// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemPickupNotification.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class SLASH_API UItemPickupNotification : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void ShowItemCollectedMessage(const FText& ItemName);

	UFUNCTION()
	void HideItemCollectedMessage();

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CollectedText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PickupNotificationAnimation;

	FTimerHandle HideNotificationTimer;
	
};
