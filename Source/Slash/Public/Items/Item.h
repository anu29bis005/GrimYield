
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};


UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ShowItemPickupNotification(AActor* OtherActor);

protected:

	virtual void BeginPlay() override;
	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	UFUNCTION(BlueprintPure)
	float TransformedSine();

	UFUNCTION(BlueprintPure)
	float TransformedCosine();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, Category = "Display")
	FText ItemDisplayName;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float Amplitude = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	float TimeConstant = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters", meta = (AllowPrivateAccess = "true"))
	bool bShouldHover = true;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickedUpEffect;
	


public:	

	UFUNCTION(BlueprintPure, Category = "Display")
	FORCEINLINE FText GetItemDisplayName() const { return ItemDisplayName; }

};
