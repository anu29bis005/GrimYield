// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ItemPickupNotification.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UItemPickupNotification::ShowItemCollectedMessage(const FText& ItemName)
{
	if (CollectedText && PickupNotificationAnimation)
	{
		CollectedText->SetText(
			FText::Format(
				NSLOCTEXT("ItemPickup", "CollectedMessage", "Collected {0}"),
				ItemName)
		);

		PlayAnimation(
			PickupNotificationAnimation,
			0.0f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}



	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideNotificationTimer);

		World->GetTimerManager().SetTimer(
			HideNotificationTimer,
			this,
			&UItemPickupNotification::HideItemCollectedMessage,
			2.0f,
			false
		);
	}
}

void UItemPickupNotification::HideItemCollectedMessage()
{
	SetVisibility(ESlateVisibility::Hidden);
}
