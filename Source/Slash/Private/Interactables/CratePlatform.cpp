
#include "Interactables/CratePlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "HUD/InteractionPromptWidget.h"
#include "Characters/Protagonist.h"
#include "Math/UnrealMathUtility.h"

ACratePlatform::ACratePlatform()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);

	PushInteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PushInteractionBox"));
	PushInteractionBox->SetupAttachment(GetRootComponent());

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	InteractionWidget->SetVisibility(false);

	InteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionPoint"));
	InteractionPoint->SetupAttachment(GetRootComponent());

	PushInteractionBox->OnComponentBeginOverlap.AddDynamic(
		this, 
		&ACratePlatform::OnBoxOverlap
	);
	
	PushInteractionBox->OnComponentEndOverlap.AddDynamic(
		this, 
		&ACratePlatform::OnBoxEndOverlap
	);

	ElevatorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorTriggerBox"));
	ElevatorTriggerBox->SetupAttachment(GetRootComponent());

	PushingCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PushingCamera"));
	PushingCamera->SetupAttachment(GetRootComponent());
}

void ACratePlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	InitialPushLocation = StartLocation + InitialPushOffset;
	StopPushLocation = StartLocation + StopPushOffset;
	FinalPushLocation = StartLocation + FinalPushOffset;
	ElevatedLocation = FinalPushLocation + ElevatorHeightOffset;

	ElevatorTriggerBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ACratePlatform::OnElevatorOverlap
	);
	
}

void ACratePlatform::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsInteractable || CurrentPushState != EPushState::EPS_Idle)
	{
		return;
	}

	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (Protagonist)
	{
		Protagonist->SetCurrentInteractable(this);

		if (InteractionWidget)
		{
			UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(InteractionWidget->GetUserWidgetObject());
			if (PromptWidget)
			{
				PromptWidget->SetInteractionType(InteractionType);
			}

			InteractionWidget->SetVisibility(true);
		}
	}
}

void ACratePlatform::OnBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (CurrentPushState != EPushState::EPS_Idle)
	{
		return;
	}

	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (Protagonist)
	{
		if (Protagonist->GetCurrentInteractable() == this)
		{
			Protagonist->SetCurrentInteractable(nullptr);
		}

		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(false);
		}
	}
}

void ACratePlatform::OnElevatorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only activate elevator if pushing is permanently finished and elevator isn't already running
	if (CurrentPushState != EPushState::EPS_Finished || bIsElevatorMoving)
	{
		return;
	}

	AProtagonist* Player = Cast<AProtagonist>(OtherActor);

	if (Player)
	{
		bIsElevated = !bIsElevated;
		bIsElevatorMoving = true;
	}

}

FVector ACratePlatform::GetInteractionLocation() const
{
	return InteractionPoint->GetComponentLocation();
}

FRotator ACratePlatform::GetInteractionRotation() const
{
	return InteractionPoint->GetComponentRotation();
}

void ACratePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLoc = GetActorLocation();

	// 1. Initial Auto-Push (Syncs with Pushing_Start montage triggered by Anim Notify)
	if (CurrentPushState == EPushState::EPS_InitialPush)
	{
		FVector NewLoc = FMath::VInterpConstantTo(
			CurrentLoc,
			InitialPushLocation,
			DeltaTime,
			PushSpeed
		);

		SetActorLocation(NewLoc);


		if (PushingProtagonist)
		{
			PushingProtagonist->AddActorWorldOffset(NewLoc - CurrentLoc);
		}

		if (CurrentLoc.Equals(InitialPushLocation, 1.f))
		{
			CurrentPushState = EPushState::EPS_ManualPush;
			if (PushingProtagonist)
			{
				PushingProtagonist->OnInitialPushFinished();
			}
		}
	}

	// 2. Manual Push (Driven only by W/D keys)
	else if (CurrentPushState == EPushState::EPS_ManualPush && CurrentPushInput > 0.01f)
	{
		FVector NewLoc = FMath::VInterpConstantTo(
			CurrentLoc,
			StopPushLocation,
			DeltaTime,
			PushSpeed * CurrentPushInput
		);

		SetActorLocation(NewLoc);

		if (PushingProtagonist)
		{
			PushingProtagonist->AddActorWorldOffset(NewLoc - CurrentLoc);
		}

		if (CurrentLoc.Equals(StopPushLocation, 1.f))
		{
			CurrentPushState = EPushState::EPS_FinalPush;

			if (PushingProtagonist)
			{
				PushingProtagonist->FinishPushing();
				PushingProtagonist = nullptr;
			}
		}
	}

	// 3. Final Auto-Glide (Syncs with the player's Pushing_Stop Montage)
	else if (CurrentPushState == EPushState::EPS_FinalPush)
	{
		FVector NewLoc = FMath::VInterpConstantTo(
			CurrentLoc,
			FinalPushLocation,
			DeltaTime,
			PushSpeed
		);

		SetActorLocation(NewLoc);

		if (CurrentLoc.Equals(FinalPushLocation, 1.f))
		{
			CurrentPushState = EPushState::EPS_Finished;
		}
	}

	// 3. Elevator Vertical Movement (Smooth Deceleration - VInterpTo)
	if (bIsElevatorMoving)
	{
		FVector TargetLoc = bIsElevated ? ElevatedLocation : FinalPushLocation;

		FVector NewLoc = FMath::VInterpTo(
			CurrentLoc,
			TargetLoc,
			DeltaTime,
			ElevatorSpeed
		);

		SetActorLocation(NewLoc);

		if (CurrentLoc.Equals(TargetLoc, 2.f))
		{
			SetActorLocation(TargetLoc);
			bIsElevatorMoving = false;
		}
	}
}

void ACratePlatform::Interact(AProtagonist* Protagonist)
{
	if (CurrentPushState != EPushState::EPS_Idle || !Protagonist)
	{
		return;
	}

	Protagonist->SetCurrentInteractable(nullptr);
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}

	PushingProtagonist = Protagonist;

	PushingProtagonist->BeginPushing(this);
}

void ACratePlatform::ReceivePushInput(float Value)
{
	CurrentPushInput = Value;
}

void ACratePlatform::StartInitialPush()
{
	CurrentPushState = EPushState::EPS_InitialPush;

	FVector CurrentLoc = GetActorLocation();

	float DistToStop = FVector::Distance(
		CurrentLoc,
		StopPushLocation
	);

	float StepDist = InitialPushOffset.Size();

	if (StepDist >= DistToStop)
	{
		InitialPushLocation = StopPushLocation;
	}

	else
	{
		InitialPushLocation = CurrentLoc + InitialPushOffset;
	}
}

void ACratePlatform::ResetPushState()
{
	CurrentPushState = EPushState::EPS_Idle;
	PushingProtagonist = nullptr;
}

void ACratePlatform::SetIsInteractable(bool bInteractable)
{
	bIsInteractable = bInteractable;
}

