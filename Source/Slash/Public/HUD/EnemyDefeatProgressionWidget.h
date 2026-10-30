// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyDefeatProgressionWidget.generated.h"

class UTextBlock;

UCLASS()
class SLASH_API UEnemyDefeatProgressionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SetEnemiesRemaining(int32 NewCount);

	UFUNCTION(BlueprintCallable)
	void SetClearanceText(const FText& NewClearanceText);


protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClearanceText;
};
