
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "EnvironmentTypes.h"
#include "InteractionType.h"
#include "CratePlatform.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UArrowComponent;
class UCameraComponent;
class AProtagonist;

UCLASS()
class SLASH_API ACratePlatform : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	ACratePlatform();
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AProtagonist* Protagonist) override;

	void ReceivePushInput(float Value);

	void StartInitialPush();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetIsInteractable(bool bInteractable);

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsInteractable = true;

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

	UFUNCTION()
	void OnElevatorOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Push;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* InteractionPoint;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PushInteractionBox;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ElevatorTriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* PushingCamera;

	/** <Platform Settings> */
	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	FVector InitialPushOffset;

	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	FVector StopPushOffset;

	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	FVector FinalPushOffset;

	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	FVector ElevatorHeightOffset;

	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	float PushSpeed = 80.f;

	UPROPERTY(EditAnywhere, Category = "Platform Settings")
	float ElevatorSpeed = 150.f;
	/** </Platform Settings> */

private:

	EPushState CurrentPushState = EPushState::EPS_Idle;

	float CurrentPushInput = 0.f;

	bool bIsElevated = false;
	bool bIsElevatorMoving = false;

	FVector StartLocation;
	FVector InitialPushLocation;
	FVector StopPushLocation;
	FVector FinalPushLocation;
	FVector ElevatedLocation;

	UPROPERTY()
	AProtagonist* PushingProtagonist;

public:	

	FVector GetInteractionLocation() const;
	FRotator GetInteractionRotation() const;



};
