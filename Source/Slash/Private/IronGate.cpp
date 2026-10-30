// Fill out your copyright notice in the Description page of Project Settings.


#include "IronGate.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AIronGate::AIronGate()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(Root);

}

void AIronGate::SetOpenProgress(float Progress)
{
	Progress = FMath::Clamp(
		Progress,
		0.f,
		1.f
	);

	const FVector NewLocation = FMath::Lerp(
		ClosedLocation,
		OpenLocation,
		Progress
	);

	GateMesh->SetRelativeLocation(NewLocation);

	if (!bFullyOpened && FMath::IsNearlyEqual(Progress, 1.f, 0.001f))
	{
		bFullyOpened = true;

		OnIronGateFullyOpened.Broadcast();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Iron Gate Fully Open"));
	}

}


void AIronGate::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedLocation = GateMesh->GetRelativeLocation();

	OpenLocation = ClosedLocation;
	OpenLocation.Z += RiseHeight;

}


/*void AIronGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

