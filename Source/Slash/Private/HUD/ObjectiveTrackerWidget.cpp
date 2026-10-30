// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ObjectiveTrackerWidget.h"
#include "Components/TextBlock.h"

void UObjectiveTrackerWidget::SetObjectiveText(const FText& Text)
{
	if (ObjectiveText)
	{
		ObjectiveText->SetText(Text);
	}
}
