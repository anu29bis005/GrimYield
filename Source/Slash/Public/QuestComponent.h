// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"



UENUM(BlueprintType) 
enum class EQuestState : uint8
{
	EQS_Inactive		UMETA(DisplayName = "Inactive"),
	EQS_OfferActive		UMETA(DisplayName = "Offer Active"),
	EQS_Active			UMETA(DisplayName = "Active"),
	EQS_Completed		UMETA(DisplayName = "Completed")
};


UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	EOS_InvestigateArea			UMETA(DisplayName = "Investigate Area"),
	EOS_FindGoldKey				UMETA(DisplayName = "Find Gold Key"),
	EOS_OpenGate				UMETA(DisplayName = "Open Gate"),
	EOS_SolveLevers				UMETA(DisplayName = "Solve Levers"),
	EOS_InvestigateChamber		UMETA(DisplayName = "Investigate Chamber"),
	EOS_OperateOldMachine		UMETA(DisplayName = "Operate Old Machine"),
	EOS_ExitDungeon				UMETA(DisplayName = "Exit Dungeon"),
	EOS_Completed				UMETA(DisplayName = "Completed")
};

UENUM(BlueprintType)
enum class EDungeonEnding : uint8
{
	EDE_None				UMETA(DisplayName = "None"),
	EDE_UntilYouArrive		UMETA(DisplayName = "Until You Arrive"),
	EDE_Alternative			UMETA(DisplayName = "Alternative")
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUntilYouArriveOfferChanged, bool);
DECLARE_MULTICAST_DELEGATE(FOnUntilYouArriveQuestAccepted);
DECLARE_MULTICAST_DELEGATE(FOnUntilYouArriveObjectiveChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UQuestComponent();


	void MarkFirstNoteRead();
	void MarkSecondNoteRead();
	void MarkGoldKeyAcquired();

	UFUNCTION(BlueprintCallable)
	void MarkFinalBossDefeat();

	void MarkNormalGateOpened();
	void MarkLeverPuzzleSolved();
	void MarkOldMachineOperated();
	void MarkIronGateOpened();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void MarkDungeonEndingStarted();

	

	void BeginUntilYouArriveOffer();
	void ExpireUntilYouArriveOffer();
	void AcceptUntilYouArriveQuest();

	void BeginUntilYouArriveHold();
	void CancelUntilYouArriveHold();

	FOnUntilYouArriveOfferChanged OnUntilYouArriveOfferChanged;
	FOnUntilYouArriveQuestAccepted OnUntilYouArriveQuestAccepted;
	FOnUntilYouArriveObjectiveChanged OnUntilYouArriveObjectiveChanged;

private:

	EQuestState QuestState = EQuestState::EQS_Inactive;

	EObjectiveState CurrentObjective = EObjectiveState::EOS_InvestigateArea;

	void RefreshUntilYouArriveObjective();
	void SetCurrentObjective(EObjectiveState NewObjective);

	bool bFirstNoteRead = false;
	bool bSecondNoteRead = false;
	bool bGoldKeyAcquired = false;
	bool bFinalBossDefeated = false;
	bool bNormalGateOpened = false;
	bool bLeverPuzzleSolved = false;
	bool bOldMachineOperated = false;
	bool bIronGateOpened = false;


	FTimerHandle UntilYouArriveOfferTimer;
	float UntilYouArriveOfferDuration = 15.f;

	FTimerHandle UntilYouArriveHoldTimer;
	float UntilYouArriveHoldDuration = 2.f;

public:	
	
	EQuestState GetQuestState() const { return QuestState;  }
		
	EObjectiveState GetCurrentObjective() const { return CurrentObjective; }

	bool HasReadFirstNote() const { return bFirstNoteRead; }
	bool HasReadSecondNote() const { return bSecondNoteRead; }
	bool HasGoldKeyBeenAcquired() const { return bGoldKeyAcquired; }
	bool HasFinalBossBeenDefeated() const { return bFinalBossDefeated; }
	bool HasNormalGateOpened() const { return bNormalGateOpened; }
	bool HasLeverPuzzleBeenSolved() const { return bLeverPuzzleSolved; }
	bool HasOldMachineBeenOperated() const { return bOldMachineOperated; }
	bool HasIronGateOpened() const { return bIronGateOpened; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetUntilYouArriveHoldProgress() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetUntilYouArriveOfferTimeRemaining() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	FText GetCurrentObjectiveText() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	EDungeonEnding GetDungeonEnding() const;
};
