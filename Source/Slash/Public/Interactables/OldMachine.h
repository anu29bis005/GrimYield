#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "InteractionType.h"
#include "OldMachine.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UArrowComponent;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneCaptureComponent2D;
class AProtagonist;
class AIronGate;

UCLASS()
class SLASH_API AOldMachine : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:

	AOldMachine();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AProtagonist* Protagonist) override;

	virtual void RefreshInteractionPrompt(AProtagonist* Protagonist) override;

	bool RotateHandle();
	void DisableInteraction();
	void ResetIncompleteInteraction(AProtagonist* Protagonist);

	void StartSplitCaptures();
	void StopSplitCaptures();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MachineBody;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HandleMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* InteractionPoint;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InteractionBox;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* MachineCamera;

	UPROPERTY(VisibleAnywhere)
	USceneCaptureComponent2D* GateCapture;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Use;

	/*OLD MACHINE ROTATION FUNCTIONS AND PROPERTIES*/

	UPROPERTY(EditAnywhere, Category = "Machine")
	float RotationPerClick = 30.f;

	UPROPERTY(EditAnywhere, Category = "Machine")
	float HandleRotationSpeed = 90.f;

	float CurrentHandleAngle = 0.f;
	float TargetHandleAngle = 0.f;

	FRotator InitialHandleRotation;

	bool canPlayerUseMachine(const AProtagonist* Player) const;
	bool bHandleRotating = false;
	bool bMachineCompleted = false;

	UFUNCTION()
	void HandleIronGateFullyOpened();

	/*GATE RESETTING FUNCTIONS AND PROPERTIES*/

	UPROPERTY(EditAnywhere, Category = "Machine|Reset")
	float GateSlamSpeed = 3.f;

	UPROPERTY(EditAnywhere, Category = "Machine|Reset")
	float HandleReturnSpeed = 180.f;

	bool bResetting = false;
	bool bGateSlamming = false;

	float ResetGateProgress = 0.f;

	TWeakObjectPtr<AProtagonist> ResettingProtagonist;


	UPROPERTY(EditInstanceOnly, Category = "Machine")
	AIronGate* IronGate = nullptr;

	

public:
	
	FVector GetInteractionLocation() const;
	FRotator GetInteractionRotation() const;
	
	FORCEINLINE bool IsMachineCompleted() const
	{
		return bMachineCompleted;
	}

};
