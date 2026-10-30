// Fill out your copyright notice in the Description page of Project Settings.


#include "Gate.h"
#include "HUD/InteractionPromptWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/Protagonist.h"
#include "Characters/GrimNPC.h"
#include "Enemy/Enemy.h"
#include "QuestComponent.h"
#include "Interactables/CratePlatform.h"

AGate::AGate()
{
 	
	PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    GateFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateFrame"));
    GateFrame->SetupAttachment(SceneRoot);

    GateDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateDoor"));
    GateDoor->SetupAttachment(GateFrame);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(SceneRoot);

    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnBoxOverlap);
    BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AGate::OnBoxEndOverlap);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(SceneRoot);

    InteractionWidget->SetVisibility(false);
}

void AGate::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator CurrentRotation = GateDoor->GetRelativeRotation();

    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetDoorRotation,
        DeltaTime,
        InterpSpeed);


    if (NewRotation.Equals(TargetDoorRotation, 0.1f))
    {
        NewRotation = TargetDoorRotation;

        SetActorTickEnabled(false);
    }

    GateDoor->SetRelativeRotation(NewRotation);
}

void AGate::Interact(AProtagonist* Protagonist)
{
    if (GateState == EGateState::EGS_Unlocked || !Protagonist)
    {
        return;
    }

    // PHASE 1: Handling Blacksmith Inspection Cutscene
    if (bRequireClothes && !Protagonist->HasInspectedCage())
    {
        if (CagedNPC)
        {
            CagedNPC->StopCryingForHelp();
        }

        Protagonist->SetHasInspectedCage(true);
        InteractionWidget->SetVisibility(false);

        if (LinkedCratePlatform)
        {
            LinkedCratePlatform->SetIsInteractable(true);
        }

        Protagonist->PlayCageCutscene();
        return;
    }

    // PHASE 2: Unlock Logic
    bool bCanUnlock = Protagonist->HasKey(RequiredKey);

    if (bRequireClothes && !Protagonist->HasClothes())
    {
        bCanUnlock = false;
    }

    if (bCanUnlock)
    {
        GateState = EGateState::EGS_Unlocked;

        if (CagedNPC)
        {
            CagedNPC->SetFreedState(true);
        }

        if (RequiredKey == EKeyType::EKT_GoldKey)
        {
            if (UQuestComponent* QuestComponent =
                Protagonist->GetQuestComponent())
            {
                QuestComponent->MarkNormalGateOpened();
            }
        }
       
        Protagonist->SetCurrentInteractable(nullptr);
        InteractionWidget->SetVisibility(false);

        TargetDoorRotation = ClosedDoorRotation;
        TargetDoorRotation.Yaw -= OpenAngle;
        SetActorTickEnabled(true);  // Setting tick only while opening
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing required item(s) for this gate."));
    }
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	
    ClosedDoorRotation = GateDoor->GetRelativeRotation();
    TargetDoorRotation = ClosedDoorRotation;

    if (GuardingEnemy)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        GuardingEnemy->OnEnemyDeath.AddDynamic(
            this,
            &AGate::OnGuardingEnemyDefeated
        );
    }
}

void AGate::OnGuardingEnemyDefeated()
{
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (GuardingEnemy)
    {
        GuardingEnemy->OnEnemyDeath.RemoveDynamic(
            this,
            &AGate::OnGuardingEnemyDefeated
        );

        GuardingEnemy = nullptr;
    }
}

void AGate::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (GateState == EGateState::EGS_Unlocked)
    {
        return;
    }


    AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

    if (!Protagonist)
    {
        return;
    }

    if (bRequireClothes && !Protagonist->HasInspectedCage())
    {
        InteractionType = EInteractionType::EIT_Inspect;
    }

    else if (Protagonist->HasKey(RequiredKey))
    {
        InteractionType = EInteractionType::EIT_Open;
    }

    else
    {
        return;
    }

    Protagonist->SetCurrentInteractable(this);

    if (InteractionWidget)
    {
        UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(InteractionWidget->GetUserWidgetObject());
        if (PromptWidget)
        {
            PromptWidget->SetInteractionType(InteractionType);
        }

        InteractionWidget->SetVisibility(true);
    }
}


void AGate::OnBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
    if (GateState == EGateState::EGS_Unlocked)
    {
        return;
    }

    AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

    if (Protagonist)
    {
        if (Protagonist->GetCurrentInteractable() == this)
        {
            Protagonist->SetCurrentInteractable(nullptr);
        }

        if (InteractionWidget)
        {
            InteractionWidget->SetVisibility(false);
        }
    }

}




