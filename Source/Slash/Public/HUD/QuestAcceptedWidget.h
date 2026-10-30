// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestAcceptedWidget.generated.h"

class UWidgetAnimation;

DECLARE_MULTICAST_DELEGATE(FOnQuestAcceptedSequenceFinished);


UCLASS()
class SLASH_API UQuestAcceptedWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void PlayQuestAcceptedSequence();

	FOnQuestAcceptedSequenceFinished OnQuestAcceptedSequenceFinished;

private:
	UFUNCTION()
	void HandleQuestAcceptedSequenceFinished();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* QuestAcceptedSequence;

};
