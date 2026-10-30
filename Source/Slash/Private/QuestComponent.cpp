// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestComponent.h"
#include "TimerManager.h"


UQuestComponent::UQuestComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

}

void UQuestComponent::MarkFirstNoteRead()
{
	bFirstNoteRead = true;

	BeginUntilYouArriveOffer();
}

void UQuestComponent::MarkSecondNoteRead()
{
	bSecondNoteRead = true;

	RefreshUntilYouArriveObjective();
}

void UQuestComponent::MarkGoldKeyAcquired()
{
	bGoldKeyAcquired = true;

	RefreshUntilYouArriveObjective();
}

void UQuestComponent::MarkFinalBossDefeat()
{
	if (bFinalBossDefeated)
	{
		return;
	}

	bFinalBossDefeated = true;

	if (QuestState != EQuestState::EQS_Active)
	{
		SetCurrentObjective(EObjectiveState::EOS_ExitDungeon);
	}

}

void UQuestComponent::MarkNormalGateOpened()
{
	bNormalGateOpened = true;

	RefreshUntilYouArriveObjective();
}

void UQuestComponent::MarkLeverPuzzleSolved()
{
	bLeverPuzzleSolved = true;

	RefreshUntilYouArriveObjective();
}

void UQuestComponent::MarkOldMachineOperated()
{
	bOldMachineOperated = true;

	RefreshUntilYouArriveObjective();
}

void UQuestComponent::MarkIronGateOpened()
{
	bIronGateOpened = true;

	if (QuestState == EQuestState::EQS_Active)
	{
		RefreshUntilYouArriveObjective();
	}

}

void UQuestComponent::MarkDungeonEndingStarted()
{
	if (QuestState == EQuestState::EQS_Active)
	{
		QuestState = EQuestState::EQS_Completed;
	}

	SetCurrentObjective(EObjectiveState::EOS_Completed);
}

void UQuestComponent::BeginUntilYouArriveOffer()
{
	if (!bFirstNoteRead)
	{
		return;
	}

	if (QuestState!=EQuestState::EQS_Inactive)
	{
		return;
	}

	QuestState = EQuestState::EQS_OfferActive;

	OnUntilYouArriveOfferChanged.Broadcast(true);

	GetWorld()->GetTimerManager().SetTimer(
		UntilYouArriveOfferTimer,
		this,
		&UQuestComponent::ExpireUntilYouArriveOffer,
		UntilYouArriveOfferDuration,
		false);
}

void UQuestComponent::ExpireUntilYouArriveOffer()
{
	if (QuestState != EQuestState::EQS_OfferActive)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(UntilYouArriveHoldTimer);

	QuestState = EQuestState::EQS_Inactive;

	OnUntilYouArriveOfferChanged.Broadcast(false);
}

void UQuestComponent::AcceptUntilYouArriveQuest()
{
	if (QuestState != EQuestState::EQS_OfferActive)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(UntilYouArriveOfferTimer);

	GetWorld()->GetTimerManager().ClearTimer(UntilYouArriveHoldTimer);

	QuestState = EQuestState::EQS_Active;

	OnUntilYouArriveOfferChanged.Broadcast(false);

	RefreshUntilYouArriveObjective();

	OnUntilYouArriveQuestAccepted.Broadcast();
}

void UQuestComponent::BeginUntilYouArriveHold()
{
	if (QuestState != EQuestState::EQS_OfferActive)
	{
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(UntilYouArriveHoldTimer))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		UntilYouArriveHoldTimer,
		this,
		&UQuestComponent::AcceptUntilYouArriveQuest,
		UntilYouArriveHoldDuration,
		false);
}

void UQuestComponent::CancelUntilYouArriveHold()
{
	GetWorld()->GetTimerManager().ClearTimer(UntilYouArriveHoldTimer);
}

void UQuestComponent::RefreshUntilYouArriveObjective()
{
	if (QuestState != EQuestState::EQS_Active)
	{
		return;
	}

	if (!bGoldKeyAcquired)
	{
		SetCurrentObjective(EObjectiveState::EOS_FindGoldKey);
	}
	else if (!bNormalGateOpened)
	{
		SetCurrentObjective(EObjectiveState::EOS_OpenGate);
	}
	else if (!bLeverPuzzleSolved)
	{
		SetCurrentObjective(EObjectiveState::EOS_SolveLevers);
	}
	else if (!bSecondNoteRead)
	{
		SetCurrentObjective(EObjectiveState::EOS_InvestigateChamber);
	}
	else if (!bIronGateOpened)
	{
		SetCurrentObjective(EObjectiveState::EOS_OperateOldMachine);
	}
	else
	{
		SetCurrentObjective(EObjectiveState::EOS_ExitDungeon);
	}
}

void UQuestComponent::SetCurrentObjective(EObjectiveState NewObjective)
{
	if (CurrentObjective == NewObjective)
	{
		return;
	}

	CurrentObjective = NewObjective;

	OnUntilYouArriveObjectiveChanged.Broadcast();
}

float UQuestComponent::GetUntilYouArriveHoldProgress() const
{
	if (QuestState != EQuestState::EQS_OfferActive)
	{
		return 0.f;
	}

	const float ElapsedTime =
		GetWorld()->GetTimerManager().GetTimerElapsed(UntilYouArriveHoldTimer);

	if (ElapsedTime < 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(
		ElapsedTime / UntilYouArriveHoldDuration,
		0.f,
		1.f);
}

float UQuestComponent::GetUntilYouArriveOfferTimeRemaining() const
{
	if (QuestState != EQuestState::EQS_OfferActive)
	{
		return 0.f;
	}

	const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(UntilYouArriveOfferTimer);

	if (RemainingTime < 0.f)
	{
		return 0.f;
	}

	return RemainingTime;
}

FText UQuestComponent::GetCurrentObjectiveText() const
{
	switch (CurrentObjective)
	{
	case EObjectiveState::EOS_InvestigateArea:
		return FText::FromString(TEXT("Explore the area."));

	case EObjectiveState::EOS_FindGoldKey:
		return FText::FromString(TEXT("Look for the Gold Key."));

	case EObjectiveState::EOS_OpenGate:
		return FText::FromString(TEXT("Search and open the Gate."));

	case EObjectiveState::EOS_SolveLevers:
		return FText::FromString(TEXT("Find out why this gate was locked."));

	case EObjectiveState::EOS_InvestigateChamber:
		return FText::FromString(TEXT("Investigate the secret chamber."));

	case EObjectiveState::EOS_OperateOldMachine:
		return FText::FromString(TEXT("Find a way out."));

	case EObjectiveState::EOS_ExitDungeon:
		return FText::FromString(TEXT("Escape the Dungeon to end the game."));

	case EObjectiveState::EOS_Completed:
	default:
		return FText::GetEmpty();
	}
}

EDungeonEnding UQuestComponent::GetDungeonEnding() const
{
	if (CurrentObjective == EObjectiveState::EOS_Completed)
	{
		return EDungeonEnding::EDE_None;
	}

	if (QuestState == EQuestState::EQS_Active)
	{
		if (bIronGateOpened)
		{
			return EDungeonEnding::EDE_UntilYouArrive;
		}

		return EDungeonEnding::EDE_None;
	}

	if (bFinalBossDefeated)
	{
		return EDungeonEnding::EDE_Alternative;
	}

	return EDungeonEnding::EDE_None;

}

