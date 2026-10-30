
#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InteractionType.h"
#include "Interfaces/InteractInterface.h"
#include "GrimNPC.generated.h"

class UAudioComponent;
class UBoxComponent;
class UWidgetComponent;


UCLASS()
class SLASH_API AGrimNPC : public ABaseCharacter, public IInteractInterface
{
	GENERATED_BODY()
	
public:

	AGrimNPC();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Interact(AProtagonist* Protagonist) override;

	void SetFreedState(bool bFreed);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopCryingForHelp();

	UFUNCTION(BlueprintCallable, Category = "Apparel")
	void EquipApparel();

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

	void PlayReleaseCutscene(AProtagonist* Protagonist);

	UPROPERTY(EditDefaultsOnly, Category = "Cinematics")
	class UAnimMontage* WearingApparelMontage;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cinematics")
	void TriggerCameraSequence();

private:
	
	void PlayCrySound();


	/** <Modular Body Parts> */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Body Parts", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Body Parts", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Body Parts", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Body Parts", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FeetMesh;
	/** </Modular Body Parts> */

	/** <Apparel> */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Character|Apparel", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShirtMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Character|Apparel", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ApronMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modular Character|Apparel", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TrousersMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Apparel")
	USkeletalMesh* GlovesMeshAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Apparel")
	USkeletalMesh* BootsMeshAsset;
	/** </Apparel> */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NPC State", meta = (AllowPrivateAccess = "true"))
	ENPCState NPCState = ENPCState::ENS_Idle;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Talk;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	UAudioComponent* CryAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "NPC State")
	bool bIsFreed = false;

	UPROPERTY(EditAnywhere, Category = "NPC State")
	bool bIsCaptive = false;

	FTimerHandle CryTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Audio")
	float MinCryInterval = 6.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	float MaxCryInterval = 15.f;

};
