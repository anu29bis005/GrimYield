
#include "Interactables/OldMachine.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Protagonist.h"
#include "HUD/InteractionPromptWidget.h"
#include "IronGate.h"
#include "Components/SceneCaptureComponent2D.h"
#include "QuestComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"

AOldMachine::AOldMachine()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MachineBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MachineBody"));
	MachineBody->SetupAttachment(Root);

	HandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleMesh"));
	HandleMesh->SetupAttachment(Root);

	InteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionPoint"));
	InteractionPoint->SetupAttachment(Root);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Root);
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(Root);
	InteractionWidget->SetVisibility(false);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(Root);

	MachineCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MachineCamera"));
	MachineCamera->SetupAttachment(CameraBoom);

	GateCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("GateCapture"));
	GateCapture->SetupAttachment(Root);

	MachineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MachineAudio"));
	MachineAudio->SetupAttachment(Root);
	MachineAudio->bAutoActivate = false;
	
}

void AOldMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	/*
	 * RESET AFTER INCOMPLETE EXIT
	 */
	if (bResetting)
	{
		// Phase 1: slam the gate shut.
		if (bGateSlamming)
		{
			ResetGateProgress = FMath::FInterpConstantTo(
				ResetGateProgress,
				0.f,
				DeltaTime,
				GateSlamSpeed
			);

			const bool bGateClosed = FMath::IsNearlyZero(
				ResetGateProgress,
				0.001f
			);

			if (bGateClosed)
			{
				ResetGateProgress = 0.f;
				bGateSlamming = false;
			}

			if (IronGate)
			{
				IronGate->SetOpenProgress(ResetGateProgress);
			}

			return;
		}

		// Phase 2: return handle to 0 degrees.
		CurrentHandleAngle = FMath::FInterpConstantTo(
			CurrentHandleAngle,
			0.f,
			DeltaTime,
			HandleReturnSpeed
		);

		const bool bHandleReturned = FMath::IsNearlyZero(
			CurrentHandleAngle,
			0.1f
		);

		if (bHandleReturned)
		{
			CurrentHandleAngle = 0.f;
		}

		FRotator NewRotation = InitialHandleRotation;
		NewRotation.Pitch += CurrentHandleAngle;

		HandleMesh->SetRelativeRotation(NewRotation);

		if (bHandleReturned)
		{
			TargetHandleAngle = 0.f;

			bResetting = false;

			SetActorTickEnabled(false);

			if (AProtagonist* Protagonist = ResettingProtagonist.Get())
			{
				Protagonist->FinishOldMachineReset();
			}
			
			ResettingProtagonist.Reset();
		}

		return;
	}

	/*
	 * NORMAL HANDLE OPERATION
	 */

	if (!bHandleRotating)
	{
		SetActorTickEnabled(false);
		return;
	}

	CurrentHandleAngle = FMath::FInterpConstantTo(
		CurrentHandleAngle,
		TargetHandleAngle,
		DeltaTime,
		HandleRotationSpeed
	);

	const bool bReachedTarget = FMath::IsNearlyEqual(
		CurrentHandleAngle,
		TargetHandleAngle,
		0.1f);

	if (bReachedTarget)
	{
		CurrentHandleAngle = TargetHandleAngle;
	}

	FRotator NewRotation = InitialHandleRotation;
	NewRotation.Pitch += CurrentHandleAngle;

	HandleMesh->SetRelativeRotation(NewRotation);

	if (IronGate)
	{
		const float OpenProgress = FMath::Abs(CurrentHandleAngle) / 360.f;

		IronGate->SetOpenProgress(OpenProgress);
	}

	if (bReachedTarget)
	{
		bHandleRotating = false;

		GetWorldTimerManager().SetTimer(
			AudioStopTimer,
			this,
			&AOldMachine::StopMachineAudio,
			0.15f,
			false
		);

		if (FMath::IsNearlyEqual(
			CurrentHandleAngle,
			-360.f,
			0.1f))
		{
			bMachineCompleted = true;

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Old Machine Completed")
			);
		}

		SetActorTickEnabled(false);
	}
}

void AOldMachine::BeginPlay()
{
	Super::BeginPlay();

	InteractionBox->OnComponentBeginOverlap.AddDynamic(
		this, 
		&AOldMachine::OnBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(
		this, 
		&AOldMachine::OnBoxEndOverlap);

	InitialHandleRotation = HandleMesh->GetRelativeRotation();

	if (IronGate)
	{
		IronGate->OnIronGateFullyOpened.AddDynamic(
			this,
			&AOldMachine::HandleIronGateFullyOpened
		);
	}

}

void AOldMachine::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (!canPlayerUseMachine(Protagonist))
	{
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(false);
		}

		return;
	}

	Protagonist->SetCurrentInteractable(this);
	
	if (InteractionWidget)
	{
		if (UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(InteractionWidget->GetUserWidgetObject()))
		{
			PromptWidget->SetInteractionType(InteractionType);
		}

		InteractionWidget->SetVisibility(true);
	}
}

void AOldMachine::OnBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	
	if (Protagonist)
	{
		if (Protagonist->GetCurrentInteractable() == this)
		{
			Protagonist->SetCurrentInteractable(nullptr);
		}
	}

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}
	
}

void AOldMachine::StopMachineAudio()
{
	if (MachineAudio)
	{
		MachineAudio->FadeOut(0.15, 0.f);
	}
}

void AOldMachine::Interact(AProtagonist* Protagonist)
{
	if (!canPlayerUseMachine(Protagonist))
	{
		return;
	}
	

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}

	Protagonist->BeginOldMachineInteraction(this);


	UE_LOG(	LogTemp, Warning, TEXT("Entering Old Machine Interaction"));
}

void AOldMachine::RefreshInteractionPrompt(AProtagonist* Protagonist)
{
	if (!canPlayerUseMachine(Protagonist))
	{
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(false);
		}

		return;
	}

	if (InteractionBox &&
		InteractionBox->IsOverlappingActor(Protagonist))
	{
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(true);
		}
	}
}

bool AOldMachine::RotateHandle()
{
	if (bMachineCompleted || bHandleRotating || bResetting)
	{
		return false;
	}
	
	GetWorldTimerManager().ClearTimer(AudioStopTimer);
	if (MachineAudio && !MachineAudio->IsPlaying())
	{
		MachineAudio->Play();
	}

	TargetHandleAngle -= RotationPerClick;

	TargetHandleAngle = FMath::Clamp(
		TargetHandleAngle,
		-360.f,
		0.f
	);

	bHandleRotating = true;
	SetActorTickEnabled(true);

	return true;
}


void AOldMachine::DisableInteraction()
{

	if (InteractionBox)
	{
		InteractionBox->SetCollisionEnabled(
			ECollisionEnabled::NoCollision);
	}

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}
}

void AOldMachine::ResetIncompleteInteraction(AProtagonist* Protagonist)
{
	if (bMachineCompleted || bResetting)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(AudioStopTimer);
	StopMachineAudio();

	// Stop any unfinished LMB rotation immediately.
	bHandleRotating = false;

	// Remember how far the gate is currently raised.
	ResetGateProgress = FMath::Clamp(
		FMath::Abs(CurrentHandleAngle) / 360.f,
		0.f,
		1.f
	);

	// Gate closes first.
	bGateSlamming =
		IronGate &&
		ResetGateProgress > KINDA_SMALL_NUMBER;

	// Handle will eventually return here.
	TargetHandleAngle = 0.f;

	bResetting = true;

	ResettingProtagonist = Protagonist;

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}

	SetActorTickEnabled(true);
}


void AOldMachine::StartSplitCaptures()
{
	if (GateCapture)
	{
		GateCapture->bCaptureEveryFrame = true;
	}
}

void AOldMachine::StopSplitCaptures()
{
	if (GateCapture)
	{
		GateCapture->bCaptureEveryFrame = false;
	}
}

/*PUBLIC GETTER FUNCTIONS*/


bool AOldMachine::canPlayerUseMachine(const AProtagonist* Player) const
{
	if (!Player || bMachineCompleted || bResetting)
	{
		return false;
	}

	const UQuestComponent* QuestComponent =
		Player->GetQuestComponent();

	if (!QuestComponent)
	{
		return false;
	}

	if (QuestComponent->GetQuestState() == EQuestState::EQS_Active)
	{
		return QuestComponent->HasReadSecondNote();
	}
	
	return true;
}

void AOldMachine::HandleIronGateFullyOpened()
{
	AProtagonist* Protagonist = Cast<AProtagonist>(
			UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if (!Protagonist)
	{
		return;
	}

	if (UQuestComponent* QuestComponent =
		Protagonist->GetQuestComponent())
	{
		QuestComponent->MarkOldMachineOperated();
		QuestComponent->MarkIronGateOpened();
	}

	UE_LOG(LogTemp, Warning, TEXT("Old Machine: Iron Gate fully opened - quest progression updated"));

}

FVector AOldMachine::GetInteractionLocation() const
{
	return InteractionPoint->GetComponentLocation();
}

FRotator AOldMachine::GetInteractionRotation() const
{
	return InteractionPoint->GetComponentRotation();
}
