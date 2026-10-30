

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionType.h"
#include "Interfaces/InteractInterface.h"
#include "Ladder.generated.h"

class UBoxComponent;
class UArrowComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class AProtagonist;

UCLASS()
class SLASH_API ALadder : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	ALadder();

	virtual void Interact(AProtagonist* Protagonist) override;

protected:

	virtual void BeginPlay() override;

	// Bottom Zone Overlaps

	UFUNCTION()
	void OnBottomBoxOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnBottomBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex
	);

	// Top Zone Overlaps

	UFUNCTION()
	void OnTopBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnTopBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LadderMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BottomInteractionBox;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* BottomInteractionPoint;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* BottomInteractionWidget;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TopInteractionBox;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* TopInteractionPoint;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* TopInteractionWidget;

	UPROPERTY(EditAnywhere)
	EInteractionType InteractionType = EInteractionType::EIT_Use;

	bool bIsPlayerAtTop = false;


public:	

	FVector GetBottomInteractionLocation() const;
	FRotator GetBottomInteractionRotation() const;
	FVector GetTopInteractionLocation() const;
	FRotator GetTopInteractionRotation() const;

	FORCEINLINE bool isPlayerAtTop() const { return bIsPlayerAtTop; }

};
