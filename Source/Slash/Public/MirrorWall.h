// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentTypes.h"
#include "MirrorWall.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class ALever;

DECLARE_MULTICAST_DELEGATE(FOnPuzzleSolved);
DECLARE_MULTICAST_DELEGATE(FOnMirrorFinishedRising);
DECLARE_MULTICAST_DELEGATE(FOnMirrorFinishedLowering);

UCLASS()
class SLASH_API AMirrorWall : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMirrorWall();
	virtual void Tick(float DeltaTime) override;
	void RaiseMirrorWall();
	void LowerMirrorWall();

	FOnPuzzleSolved OnPuzzleSolved;
	FOnMirrorFinishedRising OnMirrorFinishedRising;
	FOnMirrorFinishedLowering OnMirrorFinishedLowering;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Puzzle")
	TArray<ALever*> PuzzleLevers;

	UPROPERTY(EditInstanceOnly, Category = "Puzzle")
	TArray<ELeverState> RequiredStates;

private:

	void ValidatePuzzle();
	void HandleLeverStateChanged();


	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MirrorMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BackWallMesh;

	UPROPERTY(EditAnywhere, Category = "MirrorWall")
	float RaiseHeight = 500.f;

	UPROPERTY(EditAnywhere, Category = "MirrorWall")
	float RaiseSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LowerSpeed = 350.f;


	bool bPuzzleSolved = false;

	bool bMirrorWallMoving = false;
	bool bMirrorWallLowering = false;

	FVector InitialLocation;
	FVector TargetLocation;

public:	
	
	

};
