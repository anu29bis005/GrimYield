// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestAcceptedWidget.h"
#include "Animation/WidgetAnimation.h"

void UQuestAcceptedWidget::PlayQuestAcceptedSequence()
{

	if (!QuestAcceptedSequence)
	{
		return;
	}

	FWidgetAnimationDynamicEvent FinishedEvent;

	FinishedEvent.BindDynamic(this, &UQuestAcceptedWidget::HandleQuestAcceptedSequenceFinished);

	BindToAnimationFinished(QuestAcceptedSequence, FinishedEvent);

	PlayAnimation(QuestAcceptedSequence);
}

void UQuestAcceptedWidget::HandleQuestAcceptedSequenceFinished()
{
	OnQuestAcceptedSequenceFinished.Broadcast();
}
