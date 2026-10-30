// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GameHUD.h"
#include "HUD/ProtagonistOverlay.h"
#include "Blueprint/UserWidget.h"
#include "ContextControlWidget.h"
#include "HUD/InspectionWidget.h"
#include "JournalWidget.h"
#include "Characters/Protagonist.h"
#include "QuestComponent.h"
#include "HUD/QuestAcceptedWidget.h"
#include "HUD/ObjectiveTrackerWidget.h"
#include "HUD/EnemyDefeatProgressionWidget.h"
#include "HUD/ItemPickupNotification.h"
#include "Kismet/GameplayStatics.h"
#include "ShaderPipelineCache.h"
#include "ContentStreaming.h"

static bool bHasShownSplash = false;

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();

		if (Controller)
		{
			if (AProtagonist* Protagonist = Cast<AProtagonist>(Controller->GetPawn()))
			{
				if (UQuestComponent* QuestComponent = Protagonist->GetQuestComponent())
				{
					QuestComponent->OnUntilYouArriveOfferChanged.AddUObject(
						this,
						&AGameHUD::HandleUntilYouArriveOfferChanged);

					QuestComponent->OnUntilYouArriveQuestAccepted.AddUObject(
						this,
						&AGameHUD::HandleUntilYouArriveQuestAccepted);

					QuestComponent->OnUntilYouArriveObjectiveChanged.AddUObject(
						this,
						&AGameHUD::HandleUntilYouArriveObjectiveChanged);
				}
			}
		}


		if (Controller && ProtagonistOverlayClass)
		{
			ProtagonistOverlay = CreateWidget<UProtagonistOverlay>(Controller, ProtagonistOverlayClass);
			ProtagonistOverlay->AddToViewport();
		}

		if (Controller && LeverControlWidgetClass)
		{
			LeverControlWidget = CreateWidget<UContextControlWidget>(
				Controller,
				LeverControlWidgetClass
			);
			if (LeverControlWidget)
			{
				LeverControlWidget->AddToViewport(2);
				LeverControlWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && InspectionWidgetClass)
		{
			InspectionWidget = CreateWidget<UInspectionWidget>(
				Controller, 
				InspectionWidgetClass);
			if (InspectionWidget)
			{
				InspectionWidget->AddToViewport(1);
				InspectionWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && JournalWidgetClass)
		{
			JournalWidget = CreateWidget<UJournalWidget>(
				Controller,
				JournalWidgetClass);
			if (JournalWidget)
			{
				JournalWidget->AddToViewport(1);
				JournalWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && OldMachineSplitViewClass)
		{
			OldMachineSplitView = CreateWidget<UUserWidget>(
					Controller,
					OldMachineSplitViewClass);

			if (OldMachineSplitView)
			{
				OldMachineSplitView->AddToViewport(1);
				OldMachineSplitView->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && QuestOfferWidgetClass)
		{
			QuestOfferWidget = CreateWidget<UUserWidget>(
				Controller,
				QuestOfferWidgetClass
			);

			if (QuestOfferWidget)
			{
				QuestOfferWidget->AddToViewport(5);

				QuestOfferWidget->SetVisibility(
					ESlateVisibility::Hidden
				);
			}
		}

		if (Controller && QuestAcceptedWidgetClass)
		{
			QuestAcceptedWidget =
				CreateWidget<UQuestAcceptedWidget>(
					Controller,
					QuestAcceptedWidgetClass
				);

			if (QuestAcceptedWidget)
			{
				QuestAcceptedWidget->AddToViewport(20);

				QuestAcceptedWidget->SetVisibility(ESlateVisibility::Hidden);

				QuestAcceptedWidget->OnQuestAcceptedSequenceFinished.AddUObject(
					this,
					&AGameHUD::HandleQuestAcceptedSequenceFinished);


			}
		}

		if (Controller && ObjectiveTrackerWidgetClass)
		{
			ObjectiveTrackerWidget =
				CreateWidget<UObjectiveTrackerWidget>(
					Controller,
					ObjectiveTrackerWidgetClass
				);

			if (ObjectiveTrackerWidget)
			{
				ObjectiveTrackerWidget->AddToViewport(10);

				ObjectiveTrackerWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && EnemyDefeatProgressionWidgetClass)
		{
			EnemyDefeatProgressionWidget =
				CreateWidget<UEnemyDefeatProgressionWidget>(
					Controller,
					EnemyDefeatProgressionWidgetClass
				);

			if (EnemyDefeatProgressionWidget)
			{
				EnemyDefeatProgressionWidget->AddToViewport();

				EnemyDefeatProgressionWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (Controller && ItemPickupNotificationClass)
		{
			ItemPickupNotificationWidget =
				CreateWidget<UItemPickupNotification>(
					Controller, 
					ItemPickupNotificationClass
				);

			if (ItemPickupNotificationWidget)
			{
				ItemPickupNotificationWidget->AddToViewport();

				ItemPickupNotificationWidget->SetVisibility(ESlateVisibility::Hidden);
			}

		}

		if (Controller && SplashScreenWidgetClass && !bHasShownSplash)
		{

			bHasShownSplash = true;
			
			HideGameplayHUD();

			if (LoadingScreenWidgetClass)
			{
				LoadingScreenWidget = CreateWidget<UUserWidget>(
					Controller, 
					LoadingScreenWidgetClass
				);

				if (LoadingScreenWidget)
				{
					LoadingScreenWidget->AddToViewport(99);
				}
			}

			GetWorldTimerManager().SetTimer(
				SplashScreenTimer,
				this,
				&AGameHUD::CheckInitializationStatus,
				0.1f,
				true
			);

		}
		else
		{
			ShowMainMenuWidget();
			OnTitleMusicRestart();
		}
	}
}

void AGameHUD::FinishSplashScreen()
{
	if (SplashScreenWidget)
	{
		SplashScreenWidget->RemoveFromParent();
		SplashScreenWidget = nullptr;
	}

	ShowMainMenuWidget();
}

void AGameHUD::ShowMainMenuWidget()
{
	APlayerController* Controller =
		GetWorld()->GetFirstPlayerController();

	if (!Controller || !MainMenuWidgetClass)
	{
		return;
	}

	MainMenuWidget = CreateWidget<UUserWidget>(
		Controller,
		MainMenuWidgetClass
	);

	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport(10);

		HideGameplayHUD();

		Controller->bShowMouseCursor = true;

		FInputModeUIOnly InputMode;

		InputMode.SetWidgetToFocus(
			MainMenuWidget->TakeWidget()
		);

		InputMode.SetLockMouseToViewportBehavior(
			EMouseLockMode::DoNotLock
		);

		Controller->SetInputMode(InputMode);
	}
}

void AGameHUD::HandleUntilYouArriveObjectiveChanged()
{
	UpdateObjectiveTrackerText();
}

void AGameHUD::UpdateObjectiveTrackerText()
{
	if (!ObjectiveTrackerWidget)
	{
		return;
	}

	APlayerController* Controller = GetWorld()->GetFirstPlayerController();

	if (!Controller)
	{
		return;
	}

	if (AProtagonist* Protagonist = Cast<AProtagonist>(Controller->GetPawn()))
	{
		if (UQuestComponent* QuestComponent = Protagonist->GetQuestComponent())
		{
			ObjectiveTrackerWidget->SetObjectiveText(QuestComponent->GetCurrentObjectiveText());
		}
	}

}

void AGameHUD::StartSplashScreenSequence()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
	}

	APlayerController* Controller = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (Controller && SplashScreenWidgetClass)
	{
		SplashScreenWidget = CreateWidget<UUserWidget>(
			Controller,
			SplashScreenWidgetClass
		);

		if (SplashScreenWidget)
		{
			SplashScreenWidget->AddToViewport(20);
		}
	}

	OnPreCompilationCompleted();

	GetWorldTimerManager().SetTimer(
		SplashScreenTimer,
		this,
		&AGameHUD::FinishSplashScreen,
		9.0f,
		false
	);
}

void AGameHUD::CheckInitializationStatus()
{
	IStreamingManager::Get().BlockTillAllRequestsFinished(0.0f, true);

	if (FShaderPipelineCache::NumPrecompilesRemaining() == 0)
	{
		GetWorldTimerManager().ClearTimer(SplashScreenTimer);
		StartSplashScreenSequence();
	}
}

void AGameHUD::ShowLeverControls()
{
	if (LeverControlWidget)
	{
		LeverControlWidget->SetLeverMode();
		LeverControlWidget->SetVisibility(ESlateVisibility::Visible
		);
	}
}

void AGameHUD::ShowInspectionControls()
{
	if (LeverControlWidget)
	{
		LeverControlWidget->SetInspectionMode();
		LeverControlWidget->SetVisibility(ESlateVisibility::Visible
		);
	}
}

void AGameHUD::ShowOldMachineControls()
{
	if (LeverControlWidget)
	{
		LeverControlWidget->SetOldMachineMode();
		LeverControlWidget->SetVisibility(
			ESlateVisibility::Visible
		);
	}
}

void AGameHUD::ShowCratePushControl()
{
	if (LeverControlWidget)
	{
		LeverControlWidget->SetCratePushMode();
		LeverControlWidget->SetVisibility(
			ESlateVisibility::Visible
		);
	}
}

void AGameHUD::HideLeverControls()
{
	if (LeverControlWidget)
	{
		LeverControlWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::ShowInspection(UTexture2D* InspectionTexture)
{
	if (InspectionWidget)
	{
		InspectionWidget->SetInspectionTexture(InspectionTexture);
		InspectionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::ShowJournal(const TArray<class UTexture2D*>& Notes)
{
	if (JournalWidget)
	{
		JournalWidget->OpenJournal(Notes);
		JournalWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideJournal()
{
	if (JournalWidget)
	{
		JournalWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::HideInspection()
{
	if (InspectionWidget)
	{
		InspectionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::ShowOldMachineSplitView()
{
	if (OldMachineSplitView)
	{
		OldMachineSplitView->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideOldMachineSplitView()
{
	if (OldMachineSplitView)
	{
		OldMachineSplitView->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::ShowGameplayHUD()
{
	if (ProtagonistOverlay)
	{
		ProtagonistOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (EnemyDefeatProgressionWidget)
	{
		EnemyDefeatProgressionWidget->SetVisibility(ESlateVisibility::Visible);
	}

	TArray<AActor*> ForestEncounterActors;

	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		FName(TEXT("ForestEntranceEncounter")),
		ForestEncounterActors
	);

	if (ForestEncounterActors.Num() > 0)
	{
		ForestEncounterActors[0]->CallFunctionByNameWithArguments(
			TEXT("RefreshEntranceEnemyCount"),
			*GLog,
			nullptr,
			true
		);
	}
}

void AGameHUD::HideGameplayHUD()
{
	if (ProtagonistOverlay)
	{
		ProtagonistOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	if (EnemyDefeatProgressionWidget)
	{
		EnemyDefeatProgressionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::UpdateEnemyDefeatProgression(int32 EnemiesRemaining)
{
	if (EnemyDefeatProgressionWidget)
	{
		EnemyDefeatProgressionWidget->SetEnemiesRemaining(EnemiesRemaining);
	}
}

void AGameHUD::UpdateEnemyDefeatClearance(const FText& ClearanceText)
{
	if (EnemyDefeatProgressionWidget)
	{
		EnemyDefeatProgressionWidget->SetClearanceText(ClearanceText);
	}
}

void AGameHUD::HideEnemyDefeatProgression()
{
	if (EnemyDefeatProgressionWidget)
	{
		EnemyDefeatProgressionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::HideMainMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
	}
}

void AGameHUD::ShowPauseMenu()
{
	ShowMainMenuWidget();
}

void AGameHUD::ShowQuestOffer()
{
	if (QuestOfferWidget)
	{
		QuestOfferWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideQuestOffer()
{
	if (QuestOfferWidget)
	{
		QuestOfferWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGameHUD::HandleUntilYouArriveOfferChanged(bool bShow)
{

	SetObjectiveTrackerSuppressed(bShow);

	if (bShow)
	{
		ShowQuestOffer();
	}

	else
	{
		HideQuestOffer();
	}
}

void AGameHUD::HandleUntilYouArriveQuestAccepted()
{
	if (!QuestAcceptedWidget)
	{
		return;
	}

	QuestAcceptedWidget->SetVisibility(ESlateVisibility::Visible);

	UGameplayStatics::SetGamePaused(
		this,
		true);

	QuestAcceptedWidget->PlayQuestAcceptedSequence();

}

void AGameHUD::HandleQuestAcceptedSequenceFinished()
{
	if (QuestAcceptedWidget)
	{
		QuestAcceptedWidget->SetVisibility(ESlateVisibility::Hidden);

		UGameplayStatics::SetGamePaused(
			this,
			false
		);
	}
}

void AGameHUD::ShowObjectiveTracker()
{

	if (bObjectiveTrackerSuppressed || !ObjectiveTrackerWidget)
	{
		return;
	}

	UpdateObjectiveTrackerText();
	
	ObjectiveTrackerWidget->SetVisibility(ESlateVisibility::Visible);
}

void AGameHUD::HideObjectiveTracker()
{
	if (ObjectiveTrackerWidget)
	{
		ObjectiveTrackerWidget->SetVisibility(
			ESlateVisibility::Hidden
		);
	}
}

void AGameHUD::ToggleObjectiveTracker()
{
	if (bObjectiveTrackerSuppressed || !ObjectiveTrackerWidget)
	{
		return;
	}

	if (ObjectiveTrackerWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		HideObjectiveTracker();
	}
	else
	{
		ShowObjectiveTracker();
	}
}

void AGameHUD::SetObjectiveTrackerSuppressed(bool bSuppressed)
{
	bObjectiveTrackerSuppressed = bSuppressed;

	if (bObjectiveTrackerSuppressed)
	{
		HideObjectiveTracker();
	}
}

void AGameHUD::ShowItemCollectedNotification(const FText& ItemName)
{
	if (ItemPickupNotificationWidget)
	{
		ItemPickupNotificationWidget->ShowItemCollectedMessage(ItemName);
		ItemPickupNotificationWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
