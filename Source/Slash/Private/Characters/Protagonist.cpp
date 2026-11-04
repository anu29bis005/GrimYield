// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Protagonist.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "QuestComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Potion.h"
#include "Animation/AnimMontage.h"
#include "HUD/GameHUD.h"
#include "HUD/ProtagonistOverlay.h"
#include "HUD/GameOver.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/KeyItem.h"
#include "Chest.h"
#include "Interactables/Lever.h"
#include "Interactables/OldMachine.h"
#include "Interactables/LetterNote.h"
#include "Interactables/Ladder.h"
#include "Interactables/CratePlatform.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"


/** PUBLIC FUNCTIONS */

AProtagonist::AProtagonist()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetMesh()->CanCharacterStepUpOn = ECB_No;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(CameraBoom);

	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));

	HealthPotionProp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthPotionProp"));
	HealthPotionProp->SetupAttachment(
		GetMesh(),
		FName("LeftHandSocket")
	);
	HealthPotionProp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthPotionProp->SetGenerateOverlapEvents(false);
	HealthPotionProp->SetVisibility(false);
	
}

void AProtagonist::Tick(float DeltaTime)
{
	if (Attributes && ProtagonistOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		ProtagonistOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	if (bIsStepping && ActionState == EActionState::EAS_LadderClimbing)
	{
		float InterpSpeed = 50.f;	// 50.f means it takes exactly 0.5 seconds to travel 25 units.

		FVector CurrentLoc = GetActorLocation();

		FVector NewLoc = FMath::VInterpConstantTo(
			CurrentLoc, 
			StepTargetLocation, 
			DeltaTime, 
			InterpSpeed
		);

		SetActorLocation(NewLoc);
	}

	if (ActionState == EActionState::EAS_Pushing && InteractingPlatform)
	{
		CurrentPushPlayRate = FMath::FInterpTo(
			CurrentPushPlayRate,
			TargetPushPlayRate,
			DeltaTime,
			6.f
		);

		InteractingPlatform->ReceivePushInput(CurrentPushPlayRate);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance &&
			PushingLoopMontage &&
			AnimInstance->Montage_IsPlaying(PushingLoopMontage))
		{
			AnimInstance->Montage_SetPlayRate(
				PushingLoopMontage,
				CurrentPushPlayRate
			);
		}
		return;
	}

	/*
	 * PUZZLE TURN HANDOFF
	 * Transfers the visual TurnLeft / TurnRight montage rotation
	 * into the actual Protagonist Actor rotation.
	 */

	if (bPuzzleTurnHandoff)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && TurningMontage)
		{
			FAnimMontageInstance* MontageInstance =
				AnimInstance->GetInstanceForMontage(TurningMontage);

			if (MontageInstance)
			{
				const float MontageWeight =
					FMath::Clamp(MontageInstance->GetWeight(), 0.f, 1.f);

				const float HandoffAlpha = 1.f - MontageWeight;

				FRotator NewRotation = GetActorRotation();

				NewRotation.Yaw = FMath::Lerp(
					PuzzleTurnStartYaw,
					PuzzleTurnTargetYaw,
					HandoffAlpha
				);

				SetActorRotation(NewRotation);
			}
			else
			{
				// Turning montage has completely blended out.
				FRotator FinalRotation = GetActorRotation();

				FinalRotation.Yaw = PuzzleTurnTargetYaw;

				SetActorRotation(FinalRotation);

				bPuzzleTurnHandoff = false;
				PendingPuzzleTurnYaw = 0.f;
			}
		}
	}

	if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		PeakFallHeight = FMath::Max(PeakFallHeight, GetActorLocation().Z);
	}
}

void AProtagonist::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AProtagonist::Movement);

		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Completed, this, &AProtagonist::Movement);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Canceled, this, &AProtagonist::Movement);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProtagonist::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AProtagonist::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AProtagonist::EquipKeyPressed);
		EnhancedInputComponent->BindAction(Attack, ETriggerEvent::Triggered, this, &AProtagonist::AttackAction);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AProtagonist::Dodge);
		EnhancedInputComponent->BindAction(Heal, ETriggerEvent::Triggered, this, &AProtagonist::DrinkHealthPotion);
		EnhancedInputComponent->BindAction(LeverClockwiseAction, ETriggerEvent::Started, this, &AProtagonist::LeverClockwise);
		EnhancedInputComponent->BindAction(LeverAntiClockwiseAction, ETriggerEvent::Started, this, &AProtagonist::LeverAntiClockwise);
		EnhancedInputComponent->BindAction(LeverExitAction, ETriggerEvent::Started, this, &AProtagonist::ExitLeverInteraction);
		EnhancedInputComponent->BindAction(PauseAction,ETriggerEvent::Started,this,&AProtagonist::PausePressed);
		EnhancedInputComponent->BindAction(ObjectiveAction,ETriggerEvent::Started,this,&AProtagonist::ObjectivePressed);
		EnhancedInputComponent->BindAction(ObjectiveAction,ETriggerEvent::Completed,this,&AProtagonist::ObjectiveReleased);
		EnhancedInputComponent->BindAction(ObjectiveAction,ETriggerEvent::Canceled,this,&AProtagonist::ObjectiveReleased);
		EnhancedInputComponent->BindAction(JournalAction, ETriggerEvent::Started, this, &AProtagonist::ToggleJournal);
	}
}

void AProtagonist::Jump()
{
	if (isUnoccupied())
		Super::Jump();
}

void AProtagonist::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{

	bool bWasInteracting = false;

	PendingLever = nullptr;
	PendingLadder = nullptr;
	PendingPlatform = nullptr;

	if (bJournalOpen)
	{
		ToggleJournal();
	}

	if (InteractingChest)
	{
		CancelChestInteraction();
		bWasInteracting = true;
	}

	if (CurrentLadder)
	{
		CurrentLadder = nullptr;
		bIsStepping = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bWasInteracting = true;
	}

	if (bInspecting)
	{
		EndInspection();
	}

	if (CurrentOldMachine)
	{
		CurrentOldMachine->ResetIncompleteInteraction(this);
		CurrentOldMachine->StopSplitCaptures();
		CurrentOldMachine = nullptr;
		bWasInteracting = true;
	}

	if (CurrentLever)
	{
		CurrentLever->OnLeverRotationFinished.RemoveAll(this);
		CurrentLever->RefreshInteractionPrompt(this);
		CurrentLever = nullptr;
		bWasInteracting = true;
	}

	if (InteractingPlatform)
	{
		InteractingPlatform->ReceivePushInput(0.f);
		InteractingPlatform->SetIsInteractable(true);
		InteractingPlatform->ResetPushState();
		InteractingPlatform = nullptr;
		bWasInteracting = true;
	}

	if (bWasInteracting)
	{
		EnableCharacterMovement(true);

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetViewTarget(this);
		}

		if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			GameHUD->HideLeverControls();
			GameHUD->HideOldMachineSplitView();
			GameHUD->ShowGameplayHUD();
		}
	}

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float AProtagonist::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void AProtagonist::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}


void AProtagonist::AddSouls(ASoul* Soul)
{
	if (Attributes && ProtagonistOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		ProtagonistOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AProtagonist::AddValue(ATreasure* Treasure)
{
	if (Attributes && ProtagonistOverlay)
	{
		Attributes->AddValue(Treasure->GetValue());
		ProtagonistOverlay->SetGold(Attributes->GetValue());
	}
}

void AProtagonist::AddHealthPotion(APotion* HealthPotion)
{
	if (Attributes && ProtagonistOverlay)
	{
		Attributes->AddHealthPotion(HealthPotion->GetHealthPotions());
		ProtagonistOverlay->SetHealthPotions(Attributes->GetHealthPotions());
	}
}

void AProtagonist::AddKey(AKeyItem* KeyItem)
{

	if (KeyItem)
	{
		InventoryKeys.AddUnique(KeyItem->GetKeyType());

		if (KeyItem->GetKeyType() == EKeyType::EKT_GoldKey && QuestComponent)
		{
			QuestComponent->MarkGoldKeyAcquired();
		}

		UE_LOG(LogTemp, Warning, TEXT("Key Collected"));
	}

}

void AProtagonist::AddClothes(AClothesItem* ClothesItem)
{
	bHasClothes = true;
	UE_LOG(LogTemp, Warning, TEXT("Clothes Collected"));
}


void AProtagonist::BeginLeverCameraTransition()
{

	if (!CurrentLever)
	{
		return;
	}

	ActionState = EActionState::EAS_LeverCameraTransition;

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->SetViewTargetWithBlend(
			CurrentLever, 
			0.6f
		);

		GetWorldTimerManager().SetTimer(
			LeverCameraTransitionTimer,
			this,
			&AProtagonist::OnLeverCameraTransitionFinished,
			0.6f,
			false
		);
	}
}

void AProtagonist::BeginOldMachineCameraTransition()
{
	if (!CurrentOldMachine)
	{
		return;
	}

	ActionState = EActionState::EAS_MachineCameraTransition;

	// Starting the Gate SceneCapture.
	CurrentOldMachine->StartSplitCaptures();

	// Making the Gate swipe widget visible.
	if (AGameHUD* GameHUD = Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideGameplayHUD();
		GameHUD->ShowOldMachineSplitView();
		GameHUD->PlayOldMachineSplitEnter();
	}

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		// Real gameplay camera simultaneously blends
		// toward the OldMachine camera.
		PC->SetViewTargetWithBlend(CurrentOldMachine, 0.6f);

		GetWorldTimerManager().SetTimer(
			OldMachineCameraTransitionTimer,
			this,
			&AProtagonist::OnOldMachineCameraTransitionFinished,
			0.6f,
			false);
	}
}

void AProtagonist::BeginInspection(UTexture2D* InspectionTexture, ALetterNote* LetterNote)
{
	if (!InspectionTexture)
	{
		return;
	}

	CollectedNotes.AddUnique(InspectionTexture);

	CurrentLetterNote = LetterNote;

	bInspecting = true;

	ActionState = EActionState::EAS_Cinematic;

	EnableCharacterMovement(false);

	if (AGameHUD* GameHUD = Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideGameplayHUD();
		GameHUD->ShowInspection(InspectionTexture);
		GameHUD->ShowInspectionControls();
	}
}

void AProtagonist::EndInspection()
{
	if (!bInspecting)
	{
		return;
	}

	bInspecting = false;

	if (CurrentLetterNote && QuestComponent)
	{
		switch (CurrentLetterNote->GetLetterNoteType())
		{
		case ELetterNoteType::ELNT_FirstNote:
			QuestComponent->MarkFirstNoteRead();
			break;

		case ELetterNoteType::ELNT_SecondNote:
			QuestComponent->MarkSecondNoteRead();
			break;

		default:
			break;
		}
	}

	if (AGameHUD* GameHUD =
		Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()
		))
	{
		GameHUD->HideInspection();
		GameHUD->HideLeverControls();
		GameHUD->ShowGameplayHUD();
	}

	EnableCharacterMovement(true);

	ActionState = EActionState::EAS_Unoccupied;

	if (CurrentInteractable)
	{
		CurrentInteractable->RefreshInteractionPrompt(this);
	}

	CurrentLetterNote = nullptr;

}

void AProtagonist::BeginOldMachineInteraction(AOldMachine* OldMachine)
{
	if (!OldMachine)
	{
		return;
	}

	CurrentOldMachine = OldMachine;

	EnableCharacterMovement(false);

	MoveToInteractionPoint(
		OldMachine->GetInteractionLocation(),
		OldMachine->GetInteractionRotation()
	);
}

void AProtagonist::ExitOldMachineInteraction()
{
	if (ActionState != EActionState::EAS_MachineInteraction)
	{
		return;
	}

	if (!CurrentOldMachine)
	{
		return;
	}

	ActionState = EActionState::EAS_MachineCameraTransition;

	if (AGameHUD* GameHUD =
		Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()
		))
	{
		GameHUD->HideLeverControls();
	}

	if (CurrentOldMachine->IsMachineCompleted())
	{
		FinishOldMachineReset();
	}
	else
	{
		CurrentOldMachine->ResetIncompleteInteraction(this);
	}
}

void AProtagonist::EnableCharacterMovement(bool bEnable)
{
	if (bEnable)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->DisableMovement();
	}
}

void AProtagonist::BeginChestInteraction(AChest* Chest)
{
	if (Chest == nullptr)
	{
		return;
	}

	InteractingChest = Chest;

	EnableCharacterMovement(false);

	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		DisArm();
		return;
	}

	MoveToInteractionPoint(
		Chest->GetInteractionLocation(),
		Chest->GetInteractionRotation());
}

void AProtagonist::BeginLeverInteraction(ALever* Lever)
{
	if (!Lever)
	{
		return;
	}

	// Already holding a weapon? Disarm first
	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		PendingLever = Lever;
		DisArm();
		return;
	}

	// Safe to begin interaction

	SetCurrentLever(Lever);

	Lever->OnLeverRotationFinished.AddUObject(
		this,
		&AProtagonist::OnLeverRotationFinished);

	MoveToInteractionPoint(
		Lever->GetInteractionLocation(),
		Lever->GetInteractionRotation());
}

void AProtagonist::BeginPushing(ACratePlatform* Platform)
{
	if (!Platform)
	{
		return;
	}

	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		PendingPlatform = Platform;
		DisArm();
		return;
	}

	InteractingPlatform = Platform;
	ActionState = EActionState::EAS_Pushing;
	EnableCharacterMovement(false);

	TargetPushPlayRate = 0.f;
	CurrentPushPlayRate = 0.f;

	MoveToInteractionPoint(
		Platform->GetInteractionLocation(), 
		Platform->GetInteractionRotation()
	);
}

void AProtagonist::HandlePushStartNotify()
{
	if (InteractingPlatform)
	{
		InteractingPlatform->StartInitialPush();
	}
}

void AProtagonist::OnInitialPushFinished()
{
	if (ActionState != EActionState::EAS_Pushing)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PushingLoopMontage)
	{
		AnimInstance->Montage_Play(PushingLoopMontage);
		// Starting with the loop paused until the player presses W or D
		AnimInstance->Montage_SetPlayRate(PushingLoopMontage, 0.f);
	}

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideGameplayHUD();
		GameHUD->ShowCratePushControl();
	}
}

void AProtagonist::FinishPushing()
{
	if (ActionState != EActionState::EAS_Pushing)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PushStopMontage)
	{
		AnimInstance->Montage_Play(PushStopMontage);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(
			this,
			1.0f
		);
	}
}

void AProtagonist::HandlePushStopFinished()
{
	ActionState = EActionState::EAS_Unoccupied;
	EnableCharacterMovement(true);
	InteractingPlatform = nullptr;

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideLeverControls();
		GameHUD->ShowGameplayHUD();
	}
}

void AProtagonist::TryLadderStep(float Direction)
{
	if (bIsStepping || Direction == 0.f || !CurrentLadder) return;

	// Check for Top Exit when pressing 'W' (Up)
	if (Direction > 0.f)
	{
		float HeightDiff = FMath::Abs(GetActorLocation().Z - CurrentLadder->GetTopInteractionLocation().Z);
		if (HeightDiff <= TopExitThreshold)
		{
			ExitLadderTop();
			return;
		}
	}

	// Check for Top Exit when pressing 'S' (Up)
	if (Direction < 0.f)
	{
		float HeightDiff = FMath::Abs(GetActorLocation().Z - CurrentLadder->GetBottomInteractionLocation().Z);
		if (HeightDiff <= BottomExitThreshold)
		{
			ExitLadderBottom();
			return;
		}
	}

	bIsStepping = true;

	StepTargetLocation = GetActorLocation() + FVector(0.f, 0.f, 25.f * Direction);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* MontageToPlay = (Direction > 0.f) ? StepUpMontage : StepDownMontage;
		FName SectionName;

		if (Direction > 0.f) // Moving Up
		{
			// If Left is currently up, step with the Right. Otherwise, step with the Left.
			SectionName = bLadderLeftLegUp ? FName("StepUpRight") : FName("StepUpLeft");
		}
		else // Moving Down
		{
			// If Left is currently up, move the Left leg down. Otherwise, move the Right leg down.
			SectionName = bLadderLeftLegUp ? FName("StepDownLeft") : FName("StepDownRight");
		}

		if (MontageToPlay)
		{
			AnimInstance->Montage_Play(MontageToPlay);
			AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
		}
	}

	// Toggle the leg boolean ONCE for the next step
	bLadderLeftLegUp = !bLadderLeftLegUp;

	if (GEngine) 
		GEngine->AddOnScreenDebugMessage(
			-1, 
			2.f, 
			FColor::Yellow, 
			FString::Printf(TEXT("Next Leg Up will be: %s"), bLadderLeftLegUp ? TEXT("Left") : TEXT("Right"))
		);
}

void AProtagonist::FinishLadderStep()
{
	bIsStepping = false;
}

void AProtagonist::BeginLadderInteraction(ALadder* Ladder)
{
	if (!Ladder)
	{
		return;
	}

	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		PendingLadder = Ladder;
		DisArm();
		return;
	}

	CurrentLadder = Ladder;
	ActionState = EActionState::EAS_LadderEntering;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	GetCharacterMovement()->StopMovementImmediately();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (Ladder->isPlayerAtTop())
	{
		MoveToInteractionPoint(
			Ladder->GetTopInteractionLocation(), 
			Ladder->GetTopInteractionRotation()
		);
	}

	else
	{
		MoveToInteractionPoint(
			Ladder->GetBottomInteractionLocation(), 
			Ladder->GetBottomInteractionRotation()
		);
	}
}

void AProtagonist::FinishLadderEnter()
{
	ActionState = EActionState::EAS_LadderClimbing;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (CurrentLadder)
	{
		float StableHeightDiff = FMath::Abs(GetActorLocation().Z - CurrentLadder->GetBottomInteractionLocation().Z);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("Stable Idle HeightDiff: %f"), StableHeightDiff));
		}

	}
}

void AProtagonist::ExitLadderBottom()
{
	ActionState = EActionState::EAS_LadderExiting;

	// Choosing the section based on which leg is currently up
	if (bLadderLeftLegUp)
	{
		PlayMontageSection(LadderExitDownMontage, FName("ExitDown_LeftLeg"));
	}

	else
	{
		PlayMontageSection(LadderExitDownMontage, FName("ExitDown_RightLeg"));
	}
}

void AProtagonist::ExitLadderTop()
{
	ActionState = EActionState::EAS_LadderExiting;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && LadderExitTopMontage)
	{
		AnimInstance->Montage_Play(LadderExitTopMontage);

		// Choosing the section based on which leg is currently planted high

		FName SectionName = bLadderLeftLegUp ? FName("ExitUp_LeftLeg") : FName("ExitUp_RightLeg");
		AnimInstance->Montage_JumpToSection(SectionName, LadderExitTopMontage);
	}
}

void AProtagonist::FinishLadderExit()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	CurrentLadder = nullptr;
}


void AProtagonist::MoveToInteractionPoint(const FVector& Location, const FRotator Rotation)
{
	FVector TargetLocation = Location;
	TargetLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnReachedInteractionPoint");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 1;

	UKismetSystemLibrary::MoveComponentTo(
		GetCapsuleComponent(),
		TargetLocation,
		Rotation,
		false,
		true,
		0.35f,
		false,
		EMoveComponentAction::Move,
		LatentInfo
	);
}


/** PROTECTED FUNCTIONS */

void AProtagonist::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("EngageableTarget"));

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}

		InitializeProtagonistOverlay(PlayerController);

	}

	ActionState = EActionState::EAS_Cinematic;
	EnableCharacterMovement(false);

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (MainMenuMontage)
		{
			AnimInstance->Montage_Play(MainMenuMontage);

			AnimInstance->Montage_JumpToSection(
				FName("KneelLoop"),
				MainMenuMontage);
		}
	}
}

void AProtagonist::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// This records the starting Z-height the moment the character walks off a ledge or jumps
	if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		PeakFallHeight = GetActorLocation().Z;
	}
}

void AProtagonist::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	float FallDistance = PeakFallHeight - GetActorLocation().Z;

	if (FallDistance > SafeFallDistance) // Checking if the fall exceeded the safe threshold
	{
		// Calculating a 0.0 to 1.0 percentage based on how close the fall was to LethalFallDistance
		float DamageAlpha = FMath::Clamp(
			(FallDistance - SafeFallDistance) / (LethalFallDistance - SafeFallDistance),
			0.f,
			1.f
		);

		float DamageAmount = DamageAlpha * MaxFallDamage;	// Scaling max damage by that percentage

		UGameplayStatics::ApplyDamage(
			this,
			DamageAmount,
			GetController(),
			this,
			UDamageType::StaticClass()
		);

		if (Attributes)
		{
			if (Attributes->isAlive())
			{
				ActionState = EActionState::EAS_HitReaction;	// Player survived

				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && HardLandMontage)
				{
					AnimInstance->Montage_Play(HardLandMontage);
				}

				else
				{
					// Fallback safety timer just in case the montage isn't assigned
					FTimerHandle FallStaggerTimer;

					GetWorldTimerManager().SetTimer(
						FallStaggerTimer,
						this,
						&AProtagonist::HitReactEnd,
						0.5f,
						false
					);
				}
				
			}

			else
			{
				Die();	// Player died from the fall
			}
		}

		PeakFallHeight = 0.f; // Resetting tracker for the next fall
	}
}

void AProtagonist::Movement(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (ActionState == EActionState::EAS_LadderClimbing)
	{
		// MovementVector.Y is 1.0 for 'W' (Up) and -1.0 for 'S' (Down)
		TryLadderStep(MovementVector.Y);
		return;
	}

	// Intercept movement input for manual pushing
	if (ActionState == EActionState::EAS_Pushing && InteractingPlatform)
	{
		TargetPushPlayRate = FMath::Max(
			0.f, 
			FMath::Max(MovementVector.X, MovementVector.Y)
		);

		return;
	}

	if (ActionState != EActionState::EAS_Unoccupied) 
		return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
	
}

void AProtagonist::Look(const FInputActionValue& Value)
{

	if (ActionState == EActionState::EAS_Cinematic)
	{
		return;
	}

	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AProtagonist::EquipKeyPressed()
{

	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	if (CurrentInteractable)
	{
		CurrentInteractable->Interact(this);
		return;
	}

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)	
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon); 
	}
	else
	{
		if (CanDisarm())	
			DisArm();
		else if (CanArm())	
			Arm();
	}
}

void AProtagonist::AttackAction()
{		
	if (ActionState == EActionState::EAS_MachineInteraction)
	{
		if (CurrentOldMachine)
		{
			CurrentOldMachine->RotateHandle();
		}

		return;
	}
	Super::AttackAction();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AProtagonist::Dodge()
{
	if (isOccupied() || !HasEnoughStamina())	
		return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && ProtagonistOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		ProtagonistOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AProtagonist::DrinkHealthPotion()
{
	if (isOccupied()) 
		return;
	
	if (!Attributes || !Attributes->CanUseHealthPotion())
	{
		return;
	}

	ActionState = EActionState::EAS_Drinking;

	if (HealthPotionProp)
	{
		HealthPotionProp->SetVisibility(true);
	}

	PlayHealthDrinkMontage();

}

void AProtagonist::LeverClockwise()
{
	if (ActionState != EActionState::EAS_LeverInteraction)
	{
		return;
	}

	if (CurrentLever)
	{
		if (CurrentLever->RotateClockwise())
		{
			ActionState = EActionState::EAS_LeverRotating;
		}
	}
}

void AProtagonist::LeverAntiClockwise()
{
	if (ActionState != EActionState::EAS_LeverInteraction)
	{
		return;
	}

	if (CurrentLever)
	{
		if (CurrentLever->RotateAntiClockwise())
		{
			ActionState = EActionState::EAS_LeverRotating;
		}
	}
}

void AProtagonist::PausePressed()
{
	if (bJournalOpen)
	{
		ToggleJournal();
		return;
	}

	if (ActionState == EActionState::EAS_Cinematic)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	if (UGameplayStatics::IsGamePaused(this))
	{
		return;
	}

	if (AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD()))
	{
		UGameplayStatics::SetGamePaused(this, true);

		GameHUD->ShowPauseMenu();
	}

	UE_LOG(LogTemp, Warning, TEXT("Pause Pressed"));
}

void AProtagonist::ObjectivePressed()
{
	if (!QuestComponent)
	{
		return;
	}

	if (QuestComponent->GetQuestState() == EQuestState::EQS_OfferActive)
	{
		QuestComponent->BeginUntilYouArriveHold();
		return;
	}

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->ToggleObjectiveTracker();
	}
}

void AProtagonist::ObjectiveReleased()
{
	if (!QuestComponent)
	{
		return;
	}

	if (QuestComponent->GetQuestState() == EQuestState::EQS_OfferActive)
	{
		QuestComponent->CancelUntilYouArriveHold();
	}

	UE_LOG(LogTemp, Warning, TEXT("Objective Tab Released"));
}

void AProtagonist::BeginPuzzleSolvedSequence()
{
	if (ActionState != EActionState::EAS_LeverInteraction)
	{
		return;
	}

	if (!CurrentLever)
	{
		return;
	}

	// Preventing movement during the cinematic
	EnableCharacterMovement(false);

	// Hiding gameplay HUD and lever controls
	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideLeverControls();
		GameHUD->HideGameplayHUD();
		GameHUD->SetObjectiveTrackerSuppressed(true);
	}

	ActionState = EActionState::EAS_Cinematic;

	// Playing the exact same exit montage
	PlayLeverExitMontage();

}

void AProtagonist::EndPuzzleSolvedSequence()
{
	EnableCharacterMovement(true);

	ActionState = EActionState::EAS_Unoccupied;

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->HideLeverControls();
		GameHUD->ShowGameplayHUD();

		GameHUD->SetObjectiveTrackerSuppressed(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Cinematic Finished"));

}

void AProtagonist::ExitLeverInteraction()
{

	if (bInspecting)
	{
		EndInspection();
		return;
	}

	if (ActionState == EActionState::EAS_MachineInteraction)
	{
		ExitOldMachineInteraction();
		return;
	}

	if (ActionState != EActionState::EAS_LeverInteraction)
	{
		return;
	}

	if (!CurrentLever)
	{
		return;
	}

	AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (GameHUD)
	{
		GameHUD->HideLeverControls();
		GameHUD->ShowGameplayHUD();
	}

	ActionState = EActionState::EAS_LeverCameraTransition;

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->SetViewTargetWithBlend(this, 0.6f);

		GetWorldTimerManager().SetTimer(
			LeverCameraTransitionTimer,
			this,
			&AProtagonist::OnLeverCameraExitFinished,
			0.6f,
			false
		);
	}
}

void AProtagonist::PlayPuzzleTurn(ELeverSide LeverSide)
{
	if (LeverSide == ELeverSide::ELS_LeftWall)
	{
		PendingPuzzleTurnYaw = 90.f;
		PlayTurningMontage(FName("RightTurn"));
	}
	else
	{
		PendingPuzzleTurnYaw = -90.f;
		PlayTurningMontage(FName("LeftTurn"));
	}
}

void AProtagonist::BeginChamberTrapSequence()
{
	ActionState = EActionState::EAS_Cinematic;

	if (AGameHUD* GameHud = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHud->HideLeverControls();
		GameHud->HideGameplayHUD();

		GameHud->SetObjectiveTrackerSuppressed(true);
	}
	UE_LOG(LogTemp, Warning, TEXT("Secret Chamber Trap Cinematic Started"));
}

void AProtagonist::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquipOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AProtagonist::EndAttack()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProtagonist::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void AProtagonist::DrinkEnd()
{
	Super::DrinkEnd();

	if (HealthPotionProp)
	{
		HealthPotionProp->SetVisibility(false);
	}

	ActionState = EActionState::EAS_Unoccupied;
}

bool AProtagonist::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
	CharacterState != ECharacterState::ECS_Unequipped;
}

bool AProtagonist::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
	CharacterState != ECharacterState::ECS_Unequipped;
}

bool AProtagonist::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && 
		EquippedWeapon;
}

void AProtagonist::DisArm()
{
	PlayEquipMontage(FName("Disarm"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AProtagonist::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquipOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}





void AProtagonist::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AProtagonist::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AProtagonist::ConsumeHealthPotion()
{
	if (!Attributes)
	{
		return;
	}

	if (!Attributes->UseHealthPotion())
	{
		return;
	}

	SetHUDHealth();

	if (ProtagonistOverlay)
	{
		ProtagonistOverlay->SetHealthPotions(Attributes->GetHealthPotions());
	}
}


void AProtagonist::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AProtagonist::PlayChestOpenMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && OpenChestMontage)
	{
		AnimInstance->Montage_Play(OpenChestMontage);
	}
}

void AProtagonist::PlayLeverExitMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && LeverExitMontage)
	{
		AnimInstance->Montage_Play(LeverExitMontage);
	}
}

void AProtagonist::PlayTurningMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && TurningMontage)
	{
		AnimInstance->Montage_Play(TurningMontage);
		AnimInstance->Montage_JumpToSection(SectionName, TurningMontage);
	}
}

void AProtagonist::PlayTrapRunTurnMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && TrapRunTurnMontage)
	{
		AnimInstance->Montage_Play(TrapRunTurnMontage);
	}
}

void AProtagonist::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
	DisableCapsule();

	OnProtagonistDefeated.Broadcast();

	if (!bFinalBossEncounterActive)
	{
		ShowGameOver();
	}
}

bool AProtagonist::HasEnoughStamina()
{
	return Attributes && (Attributes->GetStamina() > Attributes->GetDodgeCost());
}

void AProtagonist::DoneEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;

	if (InteractingChest)
	{
		MoveToInteractionPoint(
			InteractingChest->GetInteractionLocation(),
			InteractingChest->GetInteractionRotation());

		return;
	}

	if (PendingLever)
	{
		ALever* Lever = PendingLever;
		PendingLever = nullptr;

		BeginLeverInteraction(Lever);
	}

	if (PendingLadder)
	{
		ALadder* Ladder = PendingLadder;
		PendingLadder = nullptr;
		BeginLadderInteraction(Ladder);
	}

	if (PendingPlatform)
	{
		ACratePlatform* Platform = PendingPlatform;
		PendingPlatform = nullptr;
		BeginPushing(Platform);
	}
}

void AProtagonist::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}



void AProtagonist::ShowGameOver()
{
	if (!GameOverClass)
	{
		return;
	}

	UGameOver* GameOverWidget = CreateWidget<UGameOver>(GetWorld(), GameOverClass);

	if (!GameOverWidget)
	{
		return;
	}

	GameOverWidget->AddToViewport();
	GameOverWidget->ShowGameOver(1.0f);

}

bool AProtagonist::isUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool AProtagonist::isOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AProtagonist::InitializeProtagonistOverlay(APlayerController* PlayerController)
{
	AGameHUD* ProtagonistHUD = Cast<AGameHUD>(PlayerController->GetHUD());
	if (ProtagonistHUD)
	{
		ProtagonistOverlay = ProtagonistHUD->GetProtagonistOverlay();
		if (ProtagonistOverlay && Attributes)
		{
			ProtagonistOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			ProtagonistOverlay->SetStaminaBarPercent(100.f);
			ProtagonistOverlay->SetGold(0.f);
			ProtagonistOverlay->SetSouls(0.f);
			ProtagonistOverlay->SetHealthPotions(Attributes->GetHealthPotions());
		}
	}
}

void AProtagonist::SetHUDHealth()
{
	if (ProtagonistOverlay && Attributes)
	{
		ProtagonistOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AProtagonist::OnMainMenuMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(
			this,
			0.75f
		);

		PlayerController->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		GetWorldTimerManager().SetTimer(
			MainMenuCameraBlendTimer,
			this,
			&AProtagonist::FinishMainMenuTransition,
			0.75f,
			false);
	}


	AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (GameHUD)
	{
		GameHUD->HideMainMenu();
	}
}

void AProtagonist::FinishMainMenuTransition()
{
	BeginCinematicState();

	PlayIntroductionSequence();

}

void AProtagonist::FinishOldMachineReset()
{
	if (!CurrentOldMachine)
	{
		return;
	}

	if (AGameHUD* GameHUD = Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->PlayOldMachineSplitExit();
	}


	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->SetViewTargetWithBlend(
			this,
			0.6f
		);

		GetWorldTimerManager().SetTimer(
			OldMachineCameraTransitionTimer,
			this,
			&AProtagonist::OnOldMachineCameraExitFinished,
			0.6f,
			false
		);
	}
}

void AProtagonist::SetEquippedWeaponVisibility(bool bVisible)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetActorHiddenInGame(!bVisible);
		EquippedWeapon->SetActorEnableCollision(bVisible);
	}
}

void AProtagonist::BeginCinematicState()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.0f);
	}

	ActionState = EActionState::EAS_Cinematic;

	EnableCharacterMovement(false);

	// Hiding the player character and their equipped weapon
	SetActorHiddenInGame(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetEquippedWeaponVisibility(false);

	Tags.Remove(FName("EngageableTarget"));

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->SetObjectiveTrackerSuppressed(true);
		GameHUD->HideGameplayHUD();
	}

	OnCinematicStateToggled.Broadcast(true);
}

void AProtagonist::EndCinematicState()
{
	ActionState = EActionState::EAS_Unoccupied;

	EnableCharacterMovement(true);

	// Unhiding the player character and their equipped weapon
	SetActorHiddenInGame(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetEquippedWeaponVisibility(true);

	Tags.AddUnique(FName("EngageableTarget"));

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->SetObjectiveTrackerSuppressed(false);
		GameHUD->ShowGameplayHUD();
	}

	OnCinematicStateToggled.Broadcast(false);
}

void AProtagonist::PlayIntroductionSequence()
{
	if (!IntroductionSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("IntroductionSequence is not assigned."));
		return;
	}

	FMovieSceneSequencePlaybackSettings PlaybackSettings;

	ALevelSequenceActor* SequenceActor = nullptr;

	ULevelSequencePlayer* SequencePlayer =
		ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			IntroductionSequence,
			PlaybackSettings,
			SequenceActor
		);

	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(
			this,
			&AProtagonist::OnIntroductionSequenceFinished
		);

		SequencePlayer->Play();
	}
}

void AProtagonist::OnIntroductionSequenceFinished()
{
	EndCinematicState();
}

void AProtagonist::PlayCageCutscene()
{
	if (!CageCutsceneSequence)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("PLAYTEST: Cage Inspected! (Cutscene skipped)"));
		}

		UE_LOG(LogTemp, Warning, TEXT("CageCutsceneSequence is not assigned."));
		return;
	}

	BeginCinematicState();

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	ALevelSequenceActor* SequenceActor = nullptr;

	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		CageCutsceneSequence,
		PlaybackSettings,
		SequenceActor
	);

	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(
			this,
			&AProtagonist::EndCinematicState
		);

		SequencePlayer->Play();
	}
}

void AProtagonist::OnReachedInteractionPoint()
{
	if (CurrentOldMachine)
	{
		BeginOldMachineCameraTransition();
		return;
	}

	if (CurrentLever)
	{
		BeginLeverCameraTransition();
		return;
	}

	if (CurrentLadder)
	{
		if (CurrentLadder->isPlayerAtTop())
		{
			PlayMontageSection(LadderEnterMontage, FName("EnterTop"));
		}
		else
		{
			PlayMontageSection(LadderEnterMontage, FName("EnterBottom"));
		}
		return;
	}

	if (InteractingPlatform)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->SetViewTargetWithBlend(
				InteractingPlatform,
				1.0f
			);
		}
			
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && PushStartMontage)
		{
			AnimInstance->Montage_Play(PushStartMontage);
		}
		return;		
	}

	if (InteractingChest)
	{
		PlayChestOpenMontage();
	}
}


void AProtagonist::ToggleJournal()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	AGameHUD* GameHUD = Cast<AGameHUD>(PC->GetHUD());
	if (!GameHUD)
	{
		return;
	}

	if (bJournalOpen)
	{
		// Closing the Journal
		bJournalOpen = false;
		ActionState = EActionState::EAS_Unoccupied;
		EnableCharacterMovement(true);

		GameHUD->HideJournal();
		GameHUD->ShowGameplayHUD();

		PC->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	else
	{
		// Opening the Journal (only if unoccupied and the player has read at least one note)
		if (ActionState != EActionState::EAS_Unoccupied || CollectedNotes.IsEmpty())
		{
			return;
		}

		bJournalOpen = true;
		ActionState = EActionState::EAS_Cinematic;
		EnableCharacterMovement(false);

		GameHUD->HideGameplayHUD();
		GameHUD->ShowJournal(CollectedNotes);

		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
}

bool AProtagonist::HasKey(EKeyType KeyToCheck) const
{
	return InventoryKeys.Contains(KeyToCheck);;
}

void AProtagonist::OnLeverCameraTransitionFinished()
{
	ActionState = EActionState::EAS_LeverInteraction;

	AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (GameHUD)
	{
		GameHUD->HideGameplayHUD();
		GameHUD->ShowLeverControls();
	}

	UE_LOG(LogTemp, Warning, TEXT("Lever Interaction Mode"));
}

void AProtagonist::OnOldMachineCameraTransitionFinished()
{

	ActionState = EActionState::EAS_MachineInteraction;

	if (AGameHUD* GameHUD =
		Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()
		))
	{
		GameHUD->ShowOldMachineControls();
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Old Machine Interaction Mode")
	);
}

void AProtagonist::OnOldMachineCameraExitFinished()
{
	ActionState = EActionState::EAS_Unoccupied;

	EnableCharacterMovement(true);

	if (AGameHUD* GameHUD =
		Cast<AGameHUD>(
			GetWorld()->GetFirstPlayerController()->GetHUD()
		))
	{
		GameHUD->HideOldMachineSplitView();
		GameHUD->ShowGameplayHUD();
	}

	if (CurrentOldMachine)
	{
		CurrentOldMachine->StopSplitCaptures();

		if (CurrentOldMachine->IsMachineCompleted())
		{
			CurrentOldMachine->DisableInteraction();

			if (CurrentInteractable == CurrentOldMachine)
			{
				CurrentInteractable = nullptr;
			}
		}
	}

	CurrentOldMachine = nullptr;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Returned from Old Machine Interaction")
	);
}

void AProtagonist::OnLeverRotationFinished()
{
	ActionState = EActionState::EAS_LeverInteraction;
}

void AProtagonist::OnLeverCameraExitFinished()
{
	PlayLeverExitMontage();
}


void AProtagonist::HandleChestOpenPhaseOne()
{
	if (InteractingChest)
	{
		InteractingChest->OpenLidPhaseOne();
	}
}

void AProtagonist::HandleChestOpenPhaseTwo()
{
	if (InteractingChest)
	{
		InteractingChest->OpenLidPhaseTwo();
	}
}

void AProtagonist::HandleChestSpawnTreasure()
{
	if (InteractingChest)
	{
		InteractingChest->SpawnTreasure();
	}
}

void AProtagonist::HandleChestInteractionFinished()
{
	EnableCharacterMovement(true);
	InteractingChest = nullptr;
}

void AProtagonist::CancelChestInteraction()
{
	if (InteractingChest)
	{
		InteractingChest->CancelOpening(this);
	}

	EnableCharacterMovement(true);
	InteractingChest = nullptr;
}

void AProtagonist::LeverExitFinished()
{

	if (CurrentLever)
	{
		CurrentLever->OnLeverRotationFinished.RemoveAll(this);

		if (ActionState != EActionState::EAS_Cinematic)
		{
			CurrentLever->RefreshInteractionPrompt(this);
		}
	}

	if (ActionState != EActionState::EAS_Cinematic)
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
	
	CurrentLever = nullptr;
	
	UE_LOG(LogTemp, Warning, TEXT("Returned from Lever Exit"));

	OnLeverExitFinished.Broadcast();
}

void AProtagonist::TurningFinished()
{
	if (FMath::IsNearlyZero(PendingPuzzleTurnYaw))
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!AnimInstance || !TurningMontage)
	{
		return;
	}

	PuzzleTurnStartYaw = GetActorRotation().Yaw;

	PuzzleTurnTargetYaw = PuzzleTurnStartYaw + PendingPuzzleTurnYaw;

	bPuzzleTurnHandoff = true;

	AnimInstance->Montage_Stop(TurnHandoffDuration, TurningMontage);

}

void AProtagonist::TrapCameraReturn()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->SetViewTargetWithBlend(
			this,
			0.75f
		);

		OnTrapCameraReturnStarted.Broadcast();
	}
}

void AProtagonist::StartMainMenuStandUp()
{
	if (!MainMenuMontage)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{

		FOnMontageEnded MontageEndedDelegate;

		MontageEndedDelegate.BindUObject(
			this,
			&AProtagonist::OnMainMenuMontageEnded);

		AnimInstance->Montage_SetEndDelegate(
			MontageEndedDelegate,
			MainMenuMontage);

		AnimInstance->Montage_SetNextSection(
			FName("KneelLoop"),
			FName("StandUp"),
			MainMenuMontage);
	}
}





