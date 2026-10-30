// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentTypes.h"
#include "InteractionType.h"
#include "Interfaces/InteractInterface.h"
#include "Gate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class AGrimNPC;
class AEnemy;
class ACratePlatform;


UCLASS()
class SLASH_API AGate : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	AGate();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AProtagonist* Protagonist) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGuardingEnemyDefeated();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate Requirements")
	EKeyType RequiredKey = EKeyType::EKT_GoldKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate Requirements")
	bool bRequireClothes = false;

	UPROPERTY(EditInstanceOnly, Category = "Gate Requirements")
	AEnemy* GuardingEnemy;

	UPROPERTY(EditInstanceOnly, Category = "Gate Requirements")
	AGrimNPC* CagedNPC;

	UPROPERTY(EditInstanceOnly, Category = "Gate Requirements")
	ACratePlatform* LinkedCratePlatform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Open;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GateFrame;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GateDoor;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(VisibleAnywhere, Category = "Gate")
	EGateState GateState = EGateState::EGS_Locked;

	UPROPERTY(EditAnywhere, Category = "Gate")
	float OpenAngle = 60.f;

	UPROPERTY(EditInstanceOnly, Category = "Gate")
	float InterpSpeed = 2.5f;

	FRotator ClosedDoorRotation;
	FRotator TargetDoorRotation;

public:	

	

};
