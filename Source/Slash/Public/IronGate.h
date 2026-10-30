// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGate.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIronGateFullyOpened);


UCLASS()
class SLASH_API AIronGate : public AActor
{
	GENERATED_BODY()
	
public:	

	AIronGate();

	void SetOpenProgress(float Progress);

	UPROPERTY(BlueprintAssignable, Category = "Gate")
	FOnIronGateFullyOpened OnIronGateFullyOpened;

protected:

	virtual void BeginPlay() override;


private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GateMesh;

	UPROPERTY(EditAnywhere, Category = "Gate")
	float RiseHeight = 360.f;

	FVector ClosedLocation;
	FVector OpenLocation;

	bool bFullyOpened = false;

public:	


};
