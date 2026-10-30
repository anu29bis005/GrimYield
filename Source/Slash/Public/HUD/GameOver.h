// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOver.generated.h"

class USoundBase;
class UWidgetAnimation;

UCLASS()
class SLASH_API UGameOver : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void ShowGameOver(float Delay);

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void SetGameOverInput();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void StartFreezeTimer();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void QuitGame();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void FreezeGame();

private:

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* GameOverAnimation;

	UPROPERTY(EditAnywhere, Category = "Game Over")
	USoundBase* GameOverSound;
};
