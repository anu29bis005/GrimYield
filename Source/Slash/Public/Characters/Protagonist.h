// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "EnvironmentTypes.h"
#include "Protagonist.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UAnimMontage;
class UQuestComponent;
class UProtagonistOverlay;
class IInteractInterface;
class UTexture2D;
class AGate;
class AChest;
class ALever;
class AItem;
class ASoul;
class APotion;
class AKeyItem;
class ATreasure;
class AOldMachine;
class ALetterNote;
class ALadder;
class ULevelSequencePlayer;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProtagonistDefeated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematiceStateToggled, bool, bIsCinematic);
DECLARE_MULTICAST_DELEGATE(FOnLeverExitFinished);
DECLARE_MULTICAST_DELEGATE(FOnTrapCameraReturnStarted);

UCLASS()
class SLASH_API AProtagonist : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AProtagonist();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddValue(ATreasure* Treasure) override;
	virtual void AddHealthPotion(APotion* HealthPotion) override;
	virtual void AddKey(AKeyItem* KeyItem) override;
	virtual void AddClothes(class AClothesItem* ClothesItem) override;

	void BeginLeverCameraTransition();
	void BeginOldMachineCameraTransition();

	void BeginInspection(UTexture2D* InspectionTexture, ALetterNote* LetterNote);
	void EndInspection();

	void BeginOldMachineInteraction(AOldMachine* OldMachine);
	void ExitOldMachineInteraction();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCinematiceStateToggled OnCinematicStateToggled;

	FOnLeverExitFinished OnLeverExitFinished;
	FOnTrapCameraReturnStarted OnTrapCameraReturnStarted;

	void EnableCharacterMovement(bool bEnable);
	void BeginChestInteraction(AChest* Chest);
	void BeginLeverInteraction(ALever* Lever);
	
	// Pushing functions //
	void BeginPushing(class ACratePlatform* Platform);
	void OnInitialPushFinished();
	void FinishPushing();

	UFUNCTION(BlueprintCallable, Category = "Platform")
	void HandlePushStartNotify();

	UFUNCTION(BlueprintCallable, Category = "Platform")
	void HandlePushStopFinished();

	// Ladder functions //

	void TryLadderStep(float Direction);

	UFUNCTION(BlueprintCallable, Category = "Ladder")
	void FinishLadderStep();

	void BeginLadderInteraction(ALadder* Ladder);

	UFUNCTION(BlueprintCallable)
	void FinishLadderEnter();

	void ExitLadderBottom();
	void ExitLadderTop();

	UFUNCTION(BlueprintCallable)
	void FinishLadderExit();

	void MoveToInteractionPoint(const FVector& Location, const FRotator Rotation);

	void BeginPuzzleSolvedSequence();
	void EndPuzzleSolvedSequence();
	void ExitLeverInteraction();
	void PlayPuzzleTurn(ELeverSide LeverSide);
	void BeginChamberTrapSequence();
	void PlayTrapRunTurnMontage();

	void FinishOldMachineReset();


	UFUNCTION(BlueprintCallable)
	void SetEquippedWeaponVisibility(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void BeginCinematicState();
	UFUNCTION(BlueprintCallable)
	void EndCinematicState();

	void PlayIntroductionSequence();
	UFUNCTION()
	void OnIntroductionSequenceFinished();

	UFUNCTION(BlueprintCallable)
	void PlayCageCutscene();

protected:
	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(
		EMovementMode PrevMovementMode,
		uint8 PreviousCustomMode
	) override;

	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* Attack;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* Heal;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JournalAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LeverClockwiseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LeverAntiClockwiseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LeverExitAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ObjectiveAction;

	/** <Callbacks for Input> */
	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipKeyPressed();
	virtual void AttackAction() override;
	void Dodge();
	void DrinkHealthPotion();
	void LeverClockwise();
	void LeverAntiClockwise();
	void PausePressed();
	void ObjectivePressed();
	void ObjectiveReleased();
	/** </Callbacks for Input> */


	/** <Combat Functions> */
	void EquipWeapon(AWeapon* Weapon);
	virtual void EndAttack() override;
	virtual void DodgeEnd() override;
	virtual void DrinkEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void DisArm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	void PlayChestOpenMontage();
	void PlayLeverExitMontage();
	void PlayTurningMontage(const FName& SectionName);
	

	virtual void Die_Implementation() override;

	bool HasEnoughStamina();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnProtagonistDefeated OnProtagonistDefeated;

	/** </Combat Functions> */



	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void ConsumeHealthPotion();
	UFUNCTION(BlueprintCallable)
	void DoneEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	/* <INTERACTABLE NOTIFY FUNCTIONS> */
	UFUNCTION(BlueprintCallable)
	void HandleChestOpenPhaseOne();
	UFUNCTION(BlueprintCallable)
	void HandleChestOpenPhaseTwo();
	UFUNCTION(BlueprintCallable)
	void HandleChestSpawnTreasure();
	UFUNCTION(BlueprintCallable)
	void HandleChestInteractionFinished();
	UFUNCTION(BlueprintCallable)
	void CancelChestInteraction();
	UFUNCTION(BlueprintCallable)
	void LeverExitFinished();
	UFUNCTION(BlueprintCallable)
	void TurningFinished();
	UFUNCTION(BlueprintCallable)
	void TrapCameraReturn();

	UFUNCTION(BlueprintCallable)
	void StartMainMenuStandUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Over")
	TSubclassOf<class UGameOver> GameOverClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Over")
	bool bFinalBossEncounterActive = false;

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void ShowGameOver();

	UPROPERTY(EditAnywhere, Category = "Cinematics")
	TObjectPtr<class ULevelSequence> IntroductionSequence;



private:
	bool isUnoccupied();
	bool isOccupied();
	void InitializeProtagonistOverlay(APlayerController* PlayerController);
	void SetHUDHealth();

	void OnMainMenuMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);

	void FinishMainMenuTransition();

	UFUNCTION()
	void OnOldMachineCameraTransitionFinished();
	UFUNCTION()
	void OnOldMachineCameraExitFinished();

	UFUNCTION()
	void OnLeverCameraTransitionFinished();
	void OnLeverRotationFinished();

	UFUNCTION()
	void OnLeverCameraExitFinished();

	UFUNCTION()
	void OnReachedInteractionPoint();

	UPROPERTY(VisibleAnywhere)
	UQuestComponent* QuestComponent;

	UPROPERTY(VisibleInstanceOnly)
	AOldMachine* CurrentOldMachine = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	AChest* InteractingChest = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	ALever* CurrentLever = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	ALever* PendingLever = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	ALadder* CurrentLadder = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	ALadder* PendingLadder = nullptr;

	UPROPERTY()
	ACratePlatform* InteractingPlatform = nullptr;

	UPROPERTY()
	ACratePlatform* PendingPlatform = nullptr;

	UPROPERTY()
	ALetterNote* CurrentLetterNote = nullptr;

	IInteractInterface* CurrentInteractable = nullptr;

	FTimerHandle LeverCameraTransitionTimer;
	FTimerHandle OldMachineCameraTransitionTimer;
	FTimerHandle MainMenuCameraBlendTimer;


	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<EKeyType> InventoryKeys;

	//bool bHasGateKey = false;

	bool bInspecting = false;

	/* <Journal Properties> */

	UPROPERTY(VisibleAnywhere, Category = "Journal")
	TArray<UTexture2D*> CollectedNotes;

	bool bJournalOpen = false;

	void ToggleJournal();

	/* </Journal Properties> */



	/* Puzzle Handoff variables */

	bool bPuzzleTurnHandoff = false;

	float PuzzleTurnStartYaw = 0.f;

	float PuzzleTurnTargetYaw = 0.f;

	float PendingPuzzleTurnYaw = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cinematic")
	float TurnHandoffDuration = 0.25f;


	/** Character Components*/
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;


	UPROPERTY(VisibleAnywhere, Category = "Potion")
	UStaticMeshComponent* HealthPotionProp;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	
	
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapon> DefaultWeaponClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* OpenChestMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HardLandMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Platform")
	UAnimMontage* PushStartMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Platform")
	UAnimMontage* PushingLoopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Platform")
	UAnimMontage* PushStopMontage;

	float TargetPushPlayRate = 0.f;
	float CurrentPushPlayRate = 0.f;

	/* <LADDER MONTAGE & VARIABLE PROPERTIES> */

	UPROPERTY(BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
	bool bIsStepping = false;

	FVector StepTargetLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Ladder")
	UAnimMontage* StepUpMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Ladder")
	UAnimMontage* StepDownMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Ladder")
	UAnimMontage* LadderEnterMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Ladder")
	UAnimMontage* LadderExitDownMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Ladder")
	UAnimMontage* LadderExitTopMontage;

	UPROPERTY(BlueprintReadWrite, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
	bool bLadderLeftLegUp = true;

	UPROPERTY(EditAnywhere, Category = "Ladder")
	float BottomExitThreshold = 115.f;

	UPROPERTY(EditAnywhere, Category = "Ladder")
	float TopExitThreshold = 115.f;

	/* </LADDER MONTAGE & VARIABLE PROPERTIES> */

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* LeverExitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* TurningMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* TrapRunTurnMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* MainMenuMontage;

	UPROPERTY()
	UProtagonistOverlay* ProtagonistOverlay;

	// Tracks the peak Z-height reached during a fall or jump
	float PeakFallHeight = 0.f;

	// Fall Damage Thresholds
	UPROPERTY(EditAnywhere, Category = "Fall Damage")
	float SafeFallDistance = 400.f;

	UPROPERTY(EditAnywhere, Category = "Fall Damage")
	float LethalFallDistance = 900.f;

	UPROPERTY(EditAnywhere, Category = "Fall Damage")
	float MaxFallDamage = 100.f;

	// Blacksmith properties

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	bool bHasClothes = false;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	bool bHasInspectedCage = false;

	UPROPERTY(EditAnywhere, Category = "Cinematics")
	TObjectPtr<class ULevelSequence> CageCutsceneSequence;

public:

	/*FORCEINLINE bool HasGateKey() const { return bHasGateKey; }
	FORCEINLINE void SetHasGateKey(bool bHasKey) { bHasGateKey = bHasKey; }*/

	bool HasKey(EKeyType KeyToCheck) const;

	FORCEINLINE void SetCurrentLever(ALever* Lever)	{	CurrentLever = Lever;	}
	FORCEINLINE ALever* GetInteractingLever() const	{	return CurrentLever;	}

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState;  }

	FORCEINLINE void SetCurrentInteractable(IInteractInterface* Interactable) { CurrentInteractable = Interactable; }
	FORCEINLINE IInteractInterface* GetCurrentInteractable() const { return CurrentInteractable; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	FORCEINLINE UQuestComponent* GetQuestComponent() const { return QuestComponent; }

	FORCEINLINE bool HasClothes() const { return bHasClothes; }
	FORCEINLINE bool HasInspectedCage() const { return bHasInspectedCage; }
	FORCEINLINE void SetHasInspectedCage(bool bInspected) { bHasInspectedCage = bInspected; }
};
