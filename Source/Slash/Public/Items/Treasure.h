// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"


class AChest;

UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Value;

	FTimerHandle EnableCollisionTimer;

	void EnablePickupCollision();

	AChest* OwningChest=nullptr;

public:

	FORCEINLINE int32 GetValue() const { return Value; }

	FORCEINLINE void SetOwningChest(AChest* Chest) { OwningChest = Chest; }
};
