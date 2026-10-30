// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionType.h"
#include "InteractionPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable)
	void SetInteractionType(EInteractionType NewInteractionType);

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PromptTextBlock;
};
