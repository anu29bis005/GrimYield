// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentTypes.h"
#include "SecretChamberSequence.generated.h"

class AMirrorWall;
class AProtagonist;
class ACameraActor;
class UCameraShakeBase;
class UBoxComponent;
class UArrowComponent;


UCLASS()
class SLASH_API ASecretChamberSequence : public AActor
{
	GENERATED_BODY()
	
public:	

	ASecretChamberSequence();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;


	void OnPuzzleSolved();
	void OnLeverExitFinished();
	void OnMirrorFinishedRising();
	void OnMirrorFinishedLowering();
	void OnTrapCameraReturnStarted();
	void StartMirrorRevealSequence();
	void FinishRevealSequence();
	void FinishTrapSequence();
	void StartPuzzleTurn();
	void OnTrapCameraBlendFinished();
	void StartTrapCollapseSequence();

	UFUNCTION()
	void OnChamberTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	FTimerHandle CameraReturnTimer;
	FTimerHandle RevealSequenceTimer;
	FTimerHandle PuzzleTurnTimer;
	FTimerHandle TrapCameraBlendTimer;
	FTimerHandle TrapSuspenseTimer;
	FTimerHandle TrapCameraReturnTimer;

	ELeverSide FinalLeverSide = ELeverSide::ELS_LeftWall;


private:

	bool bSequenceActive = false;
	bool bTrapSequenceTriggered = false;

	UPROPERTY(EditInstanceOnly, Category = "Sequence")
	AMirrorWall* MirrorWall;

	UPROPERTY()
	AProtagonist* Protagonist;

	UPROPERTY(EditInstanceOnly, Category = "Sequence")
	ACameraActor* MirrorRevealCamera;

	UPROPERTY(EditInstanceOnly, Category = "Sequence")
	TSubclassOf<UCameraShakeBase> MirrorRevealCameraShake;

	UPROPERTY(VisibleAnywhere, Category = "Sequence")
	UBoxComponent* ChamberTrigger;

	UPROPERTY(EditInstanceOnly, Category = "Sequence")
	ACameraActor* TrapCamera;

	UPROPERTY(VisibleAnywhere, Category = "Sequence")
	UArrowComponent* TrapStartPoint;

	UPROPERTY()
	UCameraShakeBase* ActiveMirrorRevealShake = nullptr;


public:	

	

};
