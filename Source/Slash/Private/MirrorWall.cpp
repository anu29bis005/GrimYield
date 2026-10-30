// Fill out your copyright notice in the Description page of Project Settings.

#include "MirrorWall.h"
#include "Components/StaticMeshComponent.h"
#include "Interactables/Lever.h"


AMirrorWall::AMirrorWall()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MirrorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MirrorMesh"));
	MirrorMesh->SetupAttachment(SceneRoot);

	BackWallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackWallMesh"));
	BackWallMesh->SetupAttachment(SceneRoot);
}


void AMirrorWall::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
	TargetLocation = InitialLocation;

	for (ALever* Lever : PuzzleLevers)
	{
		if (Lever)
		{
			Lever->OnLeverStateChanged.AddUObject(
				this,
				&AMirrorWall::HandleLeverStateChanged);
		}
	}
}

void AMirrorWall::ValidatePuzzle()
{
	if (PuzzleLevers.Num() != RequiredStates.Num())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MirrorWall: Lever array size doesn't match RequiredStates array size.")
		);

		return;
	}

	for (int32 i = 0; i < PuzzleLevers.Num(); i++)
	{
		if (!PuzzleLevers[i])
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("MirrorWall: Lever %d is nullptr."),
				i
			);

			return;
		}

		if (PuzzleLevers[i]->GetLeverState() != RequiredStates[i])
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("MirrorWall: Lever Index %d incorrect. Current = %d, Required = %d"),
				i,
				static_cast<int32>(PuzzleLevers[i]->GetLeverState()),
				static_cast<int32>(RequiredStates[i])
			);

			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("MirrorWall: Puzzle Solved!"));

	for (ALever* Lever : PuzzleLevers)
	{
		if (Lever)
		{
			Lever->DisableInteraction();
		}
	}

	OnPuzzleSolved.Broadcast();
}

void AMirrorWall::HandleLeverStateChanged()
{
	ValidatePuzzle();
}


void AMirrorWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bMirrorWallMoving)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();

	const float CurrentMoveSpeed =
		bMirrorWallLowering ? LowerSpeed : RaiseSpeed;

	FVector NewLocation = FMath::VInterpConstantTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		CurrentMoveSpeed
	);


	if (NewLocation.Equals(TargetLocation, 1.f))
	{
		NewLocation = TargetLocation;

		bMirrorWallMoving = false;

		SetActorTickEnabled(false);


		if (bMirrorWallLowering)
		{
			bMirrorWallLowering = false;

			OnMirrorFinishedLowering.Broadcast();
			
			UE_LOG(LogTemp, Warning, TEXT("MirrorWall: Finished Lowering"));

		}

		else
		{
			OnMirrorFinishedRising.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("MirrorWall: Movement Complete"));
		}
		
	}

	SetActorLocation(NewLocation);
}

void AMirrorWall::RaiseMirrorWall()
{
	if (bPuzzleSolved)
	{
		return;
	}

	bPuzzleSolved = true;

	TargetLocation = InitialLocation;

	TargetLocation.Z += RaiseHeight;

	bMirrorWallMoving = true;

	SetActorTickEnabled(true);

	UE_LOG(LogTemp, Warning, TEXT("MirrorWall: Raising"));
}

void AMirrorWall::LowerMirrorWall()
{
	if (bMirrorWallMoving)
	{
		return;
	}

	TargetLocation = InitialLocation;

	bMirrorWallLowering = true;
	bMirrorWallMoving = true;

	SetActorTickEnabled(true);

	UE_LOG(LogTemp, Warning, TEXT("MirrorWall: Lowering"));
}

