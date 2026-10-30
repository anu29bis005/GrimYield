// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UUserWidget;
class UQuestAcceptedWidget;
class UContextControlWidget;
class UInspectionWidget;
class UTexture2D;
class UObjectiveTrackerWidget;
class UEnemyDefeatProgressionWidget;
class UItemPickupNotification;
class UJournalWidget;


UCLASS()
class SLASH_API AGameHUD : public AHUD
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Startup")
	void OnPreCompilationCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Startup")
	void OnTitleMusicRestart();

	void ShowLeverControls();
	void ShowInspectionControls();
	void ShowOldMachineControls();
	void ShowCratePushControl();

	void ShowInspection(UTexture2D* InspectionTexture);

	void ShowJournal(const TArray<class UTexture2D*>& Notes);
	void HideJournal();

	void HideLeverControls();
	void HideInspection();
	void HideCratePushControl();
	
	void ShowOldMachineSplitView();
	void HideOldMachineSplitView();

	UFUNCTION(BlueprintImplementableEvent, Category = "Old Machine")
	void PlayOldMachineSplitEnter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Old Machine")
	void PlayOldMachineSplitExit();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateEnemyDefeatProgression(int32 EnemiesRemaining);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateEnemyDefeatClearance(const FText& ClearanceText);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideEnemyDefeatProgression();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideMainMenu();

	void ShowPauseMenu();

	void ShowQuestOffer();
	void HideQuestOffer();

	void HandleUntilYouArriveOfferChanged(bool bShow);
	void HandleUntilYouArriveQuestAccepted();
	void HandleQuestAcceptedSequenceFinished();

	void ShowObjectiveTracker();
	void HideObjectiveTracker();
	void ToggleObjectiveTracker();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetObjectiveTrackerSuppressed(bool bSuppressed);

	UFUNCTION(BlueprintCallable)
	void ShowItemCollectedNotification(const FText& ItemName);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Preload")
	TArray<UObject*> CriticalAssetsToPreload;

private:

	void FinishSplashScreen();
	void ShowMainMenuWidget();
	void HandleUntilYouArriveObjectiveChanged();
	void UpdateObjectiveTrackerText();

	void StartSplashScreenSequence();
	void CheckInitializationStatus();

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<class UProtagonistOverlay> ProtagonistOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UContextControlWidget> LeverControlWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UInspectionWidget> InspectionWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UJournalWidget> JournalWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UUserWidget> OldMachineSplitViewClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<class UUserWidget> SplashScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<class UUserWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UUserWidget> QuestOfferWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UQuestAcceptedWidget> QuestAcceptedWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UObjectiveTrackerWidget> ObjectiveTrackerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TSubclassOf<UEnemyDefeatProgressionWidget> EnemyDefeatProgressionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemPickupNotification> ItemPickupNotificationClass;

	UPROPERTY()
	UInspectionWidget* InspectionWidget;

	UPROPERTY()
	UJournalWidget* JournalWidget;

	UPROPERTY()
	UProtagonistOverlay* ProtagonistOverlay;

	UPROPERTY()
	UContextControlWidget* LeverControlWidget;

	UPROPERTY()
	UUserWidget* MainMenuWidget;

	UPROPERTY()
	UUserWidget* SplashScreenWidget;

	UPROPERTY()
	UUserWidget* LoadingScreenWidget;

	UPROPERTY()
	UUserWidget* QuestOfferWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Old Machine", meta = (AllowPrivateAccess = "true"))
	UUserWidget* OldMachineSplitView;

	UPROPERTY()
	UQuestAcceptedWidget* QuestAcceptedWidget;

	UPROPERTY()
	UObjectiveTrackerWidget* ObjectiveTrackerWidget;

	UPROPERTY()
	UEnemyDefeatProgressionWidget* EnemyDefeatProgressionWidget;

	UPROPERTY()
	TObjectPtr<UItemPickupNotification> ItemPickupNotificationWidget;

	bool bObjectiveTrackerSuppressed = false;

	FTimerHandle SplashScreenTimer;


public:
	
	FORCEINLINE UProtagonistOverlay* GetProtagonistOverlay() const { return ProtagonistOverlay; }

	FORCEINLINE UContextControlWidget* GetLeverControlWidget() const { return LeverControlWidget; }
};
