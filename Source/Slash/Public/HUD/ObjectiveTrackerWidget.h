// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveTrackerWidget.generated.h"

class UTextBlock;

UCLASS()
class SLASH_API UObjectiveTrackerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetObjectiveText(const FText& Text);

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveText;
};
