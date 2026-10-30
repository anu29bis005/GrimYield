// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Lever.h"
#include "HUD/InteractionPromptWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Protagonist.h"


ALever::ALever()
{
 	
	PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    LeverBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverBase"));
    LeverBase->SetupAttachment(SceneRoot);

    LeverHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverHandle"));
    LeverHandle->SetupAttachment(LeverBase);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(SceneRoot);

    LeverCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("LeverCamera"));
    LeverCamera->SetupAttachment(CameraBoom);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(SceneRoot);

    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ALever::OnBoxOverlap);
    BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ALever::OnBoxEndOverlap);

    InteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionPoint"));
    InteractionPoint->SetupAttachment(SceneRoot);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(SceneRoot);
    InteractionWidget->SetVisibility(false);
}

void ALever::Interact(AProtagonist* Protagonist)
{
    if (!bInteractionEnabled || !Protagonist || bIsOccupied)
    {
        return;
    }

    bIsOccupied = true;
    InteractionWidget->SetVisibility(false);

    Protagonist->BeginLeverInteraction(this);

    UE_LOG(LogTemp, Warning, TEXT("Entered Lever Interaction Mode"));

}

void ALever::RefreshInteractionPrompt(AProtagonist* Protagonist)
{
    bIsOccupied = false;

    if (!Protagonist)
    {
        return;
    }

    if (!bInteractionEnabled)
    {
        if (InteractionWidget)
        {
            InteractionWidget->SetVisibility(false);
        }

        return;
    }

    if (BoxCollision &&
        BoxCollision->IsOverlappingActor(Protagonist))
    {
        if (InteractionWidget)
        {
            InteractionWidget->SetVisibility(true);
        }
    }
}


void ALever::BeginPlay()
{
	Super::BeginPlay();

    if (InteractionWidget)
    {
        UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(InteractionWidget->GetUserWidgetObject());

        if (PromptWidget)
        {
            PromptWidget->SetInteractionType(InteractionType);
        }
    }
	
    InitialHandleRotation = LeverHandle->GetRelativeRotation();
    TargetHandleRotation = InitialHandleRotation;
}

void ALever::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bInteractionEnabled || bIsOccupied)
    {
        return;
    }

    AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

    if (Protagonist)
    {
        Protagonist->SetCurrentInteractable(this);

        InteractionWidget->SetVisibility(true);
    }

    UE_LOG(LogTemp, Warning, TEXT("%s : BeginOverlap"), *GetName());
}

void ALever::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

    if (Protagonist)
    {
        if (Protagonist->GetCurrentInteractable() == this)
        {
            Protagonist->SetCurrentInteractable(nullptr);
        }
    }

    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }

    UE_LOG(LogTemp, Warning, TEXT("%s : EndOverlap"), *GetName());
}

bool ALever::RotateClockwise()
{
    if (bLeverRotating)
    {
        return false;
    }

    switch (LeverState)
    {
    case ELeverState::ELS_Negative90:
        LeverState = ELeverState::ELS_Zero;
        break;

    case ELeverState::ELS_Zero:
        LeverState = ELeverState::ELS_Positive90;
        break;

    case ELeverState::ELS_Positive90:
        return false;
    }

    UpdateTargetRotationFromState();

    return true;
}

bool ALever::RotateAntiClockwise()
{
    if (bLeverRotating)
    {
        return false;
    }

    switch (LeverState)
    {
    case ELeverState::ELS_Positive90:
        LeverState = ELeverState::ELS_Zero;
        break;

    case ELeverState::ELS_Zero:
        LeverState = ELeverState::ELS_Negative90;
        break;

    case ELeverState::ELS_Negative90:
        return false;
    }

    UpdateTargetRotationFromState();

    return true;
}

void ALever::UpdateTargetRotationFromState()
{
    TargetHandleRotation = InitialHandleRotation;

    switch (LeverState)
    {
    case ELeverState::ELS_Negative90:
        TargetHandleRotation.Roll += RotationStep;
        break;

    case ELeverState::ELS_Zero:
        break;

    case ELeverState::ELS_Positive90:
        TargetHandleRotation.Roll -= RotationStep;
        break;
    }

    bLeverRotating = true;

    SetActorTickEnabled(true);
}



void ALever::DisableInteraction()
{
    bInteractionEnabled = false;

    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }
}

FVector ALever::GetInteractionLocation() const
{
    return InteractionPoint->GetComponentLocation();
}

FRotator ALever::GetInteractionRotation() const
{
    return InteractionPoint->GetComponentRotation();
}


void ALever::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bLeverRotating)
    {
        SetActorTickEnabled(false);
        return;
    }

    FRotator CurrentRotation = LeverHandle->GetRelativeRotation();

    FRotator NewRotation = FMath::RInterpConstantTo(
        CurrentRotation,
        TargetHandleRotation,
        DeltaTime,
        RotationInterpSpeed);

    if (NewRotation.Equals(TargetHandleRotation, 0.1f))
    {
        NewRotation = TargetHandleRotation;

        bLeverRotating = false;

        SetActorTickEnabled(false);

        OnLeverRotationFinished.Broadcast();

        OnLeverStateChanged.Broadcast();

        
    }

    LeverHandle->SetRelativeRotation(NewRotation);
}

