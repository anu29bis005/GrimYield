// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "EnvironmentTypes.h"
#include "KeyItem.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AKeyItem : public AItem
{
	GENERATED_BODY()

protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, Category = "Key Properties")
	EKeyType KeyType = EKeyType::EKT_GoldKey;

public:

	FORCEINLINE EKeyType GetKeyType() const { return KeyType; }
	void SetKeyType(EKeyType NewType) { KeyType = NewType; }
};
