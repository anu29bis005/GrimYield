// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Potion.generated.h"


/**
 * 
 */
UCLASS()
class SLASH_API APotion : public AItem
{
	GENERATED_BODY()
	
public:

	APotion();

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	int32 Potions = 1;

public:

	FORCEINLINE int32 GetHealthPotions() const { return Potions; }
	FORCEINLINE void SetHealthPotions(int32 NumOfHealthPotions) { Potions = NumOfHealthPotions; }
};
