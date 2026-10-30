

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentTypes.h"
#include "InteractionType.h"
#include "Interfaces/InteractInterface.h"
#include "Chest.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UArrowComponent;
class UWidgetComponent;
class USceneComponent;
class AProtagonist;

UCLASS()
class SLASH_API AChest : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	AChest();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AProtagonist* Protagonist) override;

	void OpenLidPhaseOne();
	void OpenLidPhaseTwo();
	void CancelOpening(AProtagonist* Protagonist);
	void SpawnTreasure();
	void OnTreasureCollected();
	

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Open;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	EKeyType RequiredKey = EKeyType::EKT_None;


private:

	/*========================
			Components
	========================*/
	/** Root */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	USceneComponent* SceneRoot;

	/** Chest Base */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UStaticMeshComponent* ChestMesh;

	/** Chest Lid */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UStaticMeshComponent* ChestLid;

	/** Player Interaction */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UBoxComponent* BoxCollision;

	/** Interaction Point */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UArrowComponent* InteractionPoint;

	/** Treasure Spawn Location */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UArrowComponent* TreasureSpawnPoint;

	/** Press E Widget */
	UPROPERTY(VisibleAnywhere, Category = "Chest")
	UWidgetComponent* InteractionWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest", meta = (AllowPrivateAccess = "true"))
	EChestState ChestState = EChestState::ECS_Closed;

	UPROPERTY(EditAnywhere, Category = "Chest | Treasure")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "Chest | Treasure")
	TSubclassOf<class AWeapon> WeaponClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "Chest | Treasure")
	FRotator WeaponSpawnRotationOffset = FRotator::ZeroRotator;

	bool bTreasureSpawned = false;

	FTimerHandle DestroyChestTimer;

	/** Lid Animation */

	bool bLidAnimating = false;
	float TargetLidRoll = 0.f;
	FRotator ClosedLidRotation;

	UPROPERTY(EditAnywhere, Category = "Chest|Interpolation")
	float LidInterpSpeed = 5.f;

	/** Chest Fade-out */
	UPROPERTY()
	UMaterialInstanceDynamic* ChestMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* LidMaterial = nullptr;

	float CurrentFade = 1.f;
	bool bFadingOut = false;

	UPROPERTY(EditAnywhere, Category = "Chest|Interpolation")
	float FadeInterpSpeed = 2.f;

	/*========================
			Functions
	========================*/
	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateLidAnimation(float DeltaTime);
	void UpdateFade(float DeltaTime);
	void DestroyChest();

public:

	FVector GetInteractionLocation() const;
	FRotator GetInteractionRotation() const;
};
