// Fill out your copyright notice in the Description page of Project Settings.


#include "SecretChamberSequence.h"
#include "Interactables/Lever.h"
#include "MirrorWall.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/Protagonist.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "QuestComponent.h"


ASecretChamberSequence::ASecretChamberSequence()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	ChamberTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ChamberTrigger"));
	RootComponent = ChamberTrigger;

	ChamberTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChamberTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChamberTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ChamberTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASecretChamberSequence::OnChamberTriggerOverlap);

	TrapStartPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TrapStartPoint"));
	TrapStartPoint->SetupAttachment(GetRootComponent());

}


void ASecretChamberSequence::BeginPlay()
{
	Super::BeginPlay();

	Protagonist = Cast<AProtagonist>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if (!Protagonist)
	{
		UE_LOG(LogTemp, Error,
			TEXT("SecretChamberSequence: Failed to find Protagonist."));
		return;
	}

	if (!MirrorWall)
	{
		UE_LOG(LogTemp, Error,
			TEXT("SecretChamberSequence: MirrorWall reference is not assigned."));
		return;
	}
	
	MirrorWall->OnPuzzleSolved.AddUObject(
		this,
		&ASecretChamberSequence::OnPuzzleSolved);

	Protagonist->OnLeverExitFinished.AddUObject(
		this,
		&ASecretChamberSequence::OnLeverExitFinished);

	MirrorWall->OnMirrorFinishedRising.AddUObject(
		this,
		&ASecretChamberSequence::OnMirrorFinishedRising);

	MirrorWall->OnMirrorFinishedLowering.AddUObject(
		this,
		&ASecretChamberSequence::OnMirrorFinishedLowering);

	Protagonist->OnTrapCameraReturnStarted.AddUObject(
		this,
		&ASecretChamberSequence::OnTrapCameraReturnStarted);

}

void ASecretChamberSequence::OnPuzzleSolved()
{
	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Sequence Started"));

	bSequenceActive = true;

	if (Protagonist)
	{
		if (UQuestComponent* QuestComponent =
			Protagonist->GetQuestComponent())
		{
			QuestComponent->MarkLeverPuzzleSolved();
		}

		if (ALever* FinalLever = Protagonist->GetInteractingLever())
		{
			FinalLeverSide = FinalLever->GetLeverSide();
		}

		APlayerController* PC =	UGameplayStatics::GetPlayerController(this, 0);

		if (PC && MirrorRevealCamera)
		{
		PC->SetViewTargetWithBlend(
			MirrorRevealCamera,
			0.75f);
		}
	
		Protagonist->BeginPuzzleSolvedSequence();
	}
}

void ASecretChamberSequence::OnLeverExitFinished()
{
	if (!bSequenceActive)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber: Lever Exit Finished"));

	APlayerController* PC =
		UGameplayStatics::GetPlayerController(this, 0);

	if (PC && PC->PlayerCameraManager && MirrorRevealCameraShake)
	{
		ActiveMirrorRevealShake = PC->PlayerCameraManager->StartCameraShake(
			MirrorRevealCameraShake,
			1.f,
			ECameraShakePlaySpace::CameraLocal,
			FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(
		RevealSequenceTimer,
		this,
		&ASecretChamberSequence::StartMirrorRevealSequence,
		2.0f,
		false);
}



void ASecretChamberSequence::OnMirrorFinishedRising()
{
	UE_LOG(LogTemp, Warning, TEXT("Mirror finished rising."));

	if (!Protagonist)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC)
	{
		if (PC->PlayerCameraManager && ActiveMirrorRevealShake)
		{
			PC->PlayerCameraManager->StopCameraShake(
				ActiveMirrorRevealShake,
				false
			);

			ActiveMirrorRevealShake = nullptr;

		}

		PC->SetViewTargetWithBlend(
			Protagonist,
			0.75f);

		GetWorldTimerManager().SetTimer(
			CameraReturnTimer,
			this,
			&ASecretChamberSequence::FinishRevealSequence,
			0.75f,
			false);
	}
}

void ASecretChamberSequence::OnMirrorFinishedLowering()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("MirrorWall finished lowering.")
	);

	APlayerController* PC =
		UGameplayStatics::GetPlayerController(this, 0);

	if (PC &&
		PC->PlayerCameraManager &&
		ActiveMirrorRevealShake)
	{
		PC->PlayerCameraManager->StopCameraShake(
			ActiveMirrorRevealShake,
			false);

		ActiveMirrorRevealShake = nullptr;
	}
}

void ASecretChamberSequence::OnTrapCameraReturnStarted()
{
	GetWorldTimerManager().SetTimer(
		TrapCameraReturnTimer,
		this,
		&ASecretChamberSequence::FinishTrapSequence,
		0.75f,
		false);
}

void ASecretChamberSequence::StartMirrorRevealSequence()
{

	UE_LOG(LogTemp, Warning, TEXT("Mirror Reveal Sequence Begins"));

	if (MirrorWall)
	{
		MirrorWall->RaiseMirrorWall();
	}

	GetWorldTimerManager().SetTimer(
		PuzzleTurnTimer,
		this,
		&ASecretChamberSequence::StartPuzzleTurn,
		0.25f,
		false);
}

void ASecretChamberSequence::FinishRevealSequence()
{

	if (Protagonist)
	{
		Protagonist->EndPuzzleSolvedSequence();
	}

	bSequenceActive = false;
	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Sequence Complete"));
}

void ASecretChamberSequence::FinishTrapSequence()
{
	if (Protagonist)
	{
		Protagonist->EndPuzzleSolvedSequence();
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Secret Chamber Trap Sequence Complete")
	);
}

void ASecretChamberSequence::StartPuzzleTurn()
{
	if (Protagonist)
	{
		Protagonist->PlayPuzzleTurn(FinalLeverSide);
	}
}

void ASecretChamberSequence::OnTrapCameraBlendFinished()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC && PC->PlayerCameraManager && MirrorRevealCameraShake)
	{
		ActiveMirrorRevealShake = PC->PlayerCameraManager->StartCameraShake(
			MirrorRevealCameraShake,
			1.0f,
			ECameraShakePlaySpace::CameraLocal,
			FRotator::ZeroRotator
		);

		UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Trap: Camera shake started"));

		GetWorldTimerManager().SetTimer(
			TrapSuspenseTimer,
			this,
			&ASecretChamberSequence::StartTrapCollapseSequence,
			2.f,
			false);

	}
}

void ASecretChamberSequence::StartTrapCollapseSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Trap: Collapse sequence begins"));
	
	if (Protagonist)
	{
		Protagonist->PlayTrapRunTurnMontage();
	}
	if (MirrorWall)
	{
		MirrorWall->LowerMirrorWall();
	}
}

void ASecretChamberSequence::OnChamberTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTrapSequenceTriggered)
	{
		return;
	}

	AProtagonist* EnteringProtagonist = Cast<AProtagonist>(OtherActor);

	if (!EnteringProtagonist)
	{
		return;
	}

	FVector TrapStartLocation =
		TrapStartPoint->GetComponentLocation();

	TrapStartLocation.Z += EnteringProtagonist
		->GetCapsuleComponent()
		->GetScaledCapsuleHalfHeight();

	EnteringProtagonist->SetActorLocationAndRotation(
		TrapStartLocation, TrapStartPoint->GetComponentRotation());

	bTrapSequenceTriggered = true;

	EnteringProtagonist->BeginChamberTrapSequence();

	APlayerController* PC =	UGameplayStatics::GetPlayerController(this, 0);

	if (PC && TrapCamera)
	{
		PC->SetViewTargetWithBlend(
			TrapCamera,
			0.75
		);

		GetWorldTimerManager().SetTimer(
			TrapCameraBlendTimer,
			this,
			&ASecretChamberSequence::OnTrapCameraBlendFinished,
			0.75f,
			false);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Trap Triggered"));
}


void ASecretChamberSequence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

