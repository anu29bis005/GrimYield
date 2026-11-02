

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "InteractionType.h"
#include "LetterNote.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UTexture2D;
class AProtagonist;
class AEnemy;

UENUM(BlueprintType)
enum class ELetterNoteType : uint8
{
	ELNT_None			UMETA(DisplayName = "None"),
	ELNT_FirstNote		UMETA(DisplayName = "First Note"),
	ELNT_SecondNote		UMETA(DisplayName = "Second Note")
};

UCLASS()
class SLASH_API ALetterNote : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	

	ALetterNote();

	virtual void Interact(AProtagonist* Protagonist) override;

	virtual void RefreshInteractionPrompt(AProtagonist* Protagonist) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGuardianDeath();

private:

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* NoteMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InteractionBox;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

	UPROPERTY(EditInstanceOnly, Category = "Inspection")
	UTexture2D* InspectionTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note", meta = (AllowPrivateAccess = "true"))
	ELetterNoteType LetterNoteType = ELetterNoteType::ELNT_None;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::EIT_Read;

private:

	UPROPERTY(EditInstanceOnly, Category = "Interaction")
	AEnemy* GuardianEnemy;

public:	

	FORCEINLINE ELetterNoteType GetLetterNoteType() const { return LetterNoteType; }
};
