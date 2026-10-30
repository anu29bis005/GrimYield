// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/InteractionPromptWidget.h"
#include "Components/TextBlock.h"

void UInteractionPromptWidget::SetInteractionType(EInteractionType NewInteractionType)
{
	if (!PromptTextBlock)
	{
		return;
	}

	FText PromptText;
	switch (NewInteractionType)
	{
	case EInteractionType::EIT_Open:
		PromptText = FText::FromString(TEXT("Open"));
		break;
	case EInteractionType::EIT_Use:
		PromptText = FText::FromString(TEXT("Use"));
		break;
	case EInteractionType::EIT_Push:
		PromptText = FText::FromString(TEXT("Push"));
		break;
	case EInteractionType::EIT_Pull:
		PromptText = FText::FromString(TEXT("Pull"));
		break;
	case EInteractionType::EIT_Pickup:
		PromptText = FText::FromString(TEXT("Pickup"));
		break;
	case EInteractionType::EIT_Talk:
		PromptText = FText::FromString(TEXT("Talk"));
		break;
	case EInteractionType::EIT_Inspect:
		PromptText = FText::FromString(TEXT("Inspect"));
		break;
	case EInteractionType::EIT_Read:
		PromptText = FText::FromString(TEXT("Read"));
		break;
	default:
		PromptText = FText::GetEmpty();
		break;
	}

	PromptTextBlock->SetText(PromptText);
}
