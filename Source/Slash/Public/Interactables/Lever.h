// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "EnvironmentTypes.h"
#include "InteractionType.h"
#include "Lever.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;

DECLARE_MULTICAST_DELEGATE(FOnLeverStateChanged);
DECLARE_MULTICAST_DELEGATE(FOnLeverRotationFinished);

UCLASS()
class SLASH_API ALever : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	ALever();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AProtagonist* Protagonist) override;
	virtual void RefreshInteractionPrompt(AProtagonist* Protagonist) override;

	bool RotateClockwise();
	bool RotateAntiClockwise();
	void DisableInteraction();

	FOnLeverStateChanged OnLeverStateChanged;
	FOnLeverRotationFinished OnLeverRotationFinished;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Use;

	UPROPERTY(VisibleAnywhere, Category = "Lever")
	ELeverState LeverState = ELeverState::ELS_Zero;

	
private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeverBase;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeverHandle;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* LeverCamera;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* InteractionPoint;

	/*
	LEVER PROPERTIES & FUNCTIONS
	*/
	
	bool bInteractionEnabled = true;
	bool bIsOccupied = false;
	void UpdateTargetRotationFromState();

	UPROPERTY(EditInstanceOnly, Category = "Lever")
	ELeverSide LeverSide = ELeverSide::ELS_LeftWall;

	UPROPERTY(EditAnywhere, Category = "Lever")
	float RotationStep = 90.f;

	UPROPERTY(EditAnywhere, Category = "Lever")
	float RotationInterpSpeed = 40.f;

	bool bLeverRotating = false;

	FRotator InitialHandleRotation;

	FRotator TargetHandleRotation;




public:	

	FORCEINLINE ELeverSide GetLeverSide() const	{	return LeverSide;	}

	FORCEINLINE UCameraComponent* GetLeverCamera() const { return LeverCamera; }

	FVector GetInteractionLocation() const;
	FRotator GetInteractionRotation() const;

	FORCEINLINE ELeverState GetLeverState() const { return LeverState; }
};
