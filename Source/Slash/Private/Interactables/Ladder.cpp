
#include "Interactables/Ladder.h"
#include "HUD/InteractionPromptWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/Protagonist.h"

ALadder::ALadder()
{

	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(SceneRoot);

	// BOTTOM ZONE SETUP
	BottomInteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomInteractionBox"));
	BottomInteractionBox->SetupAttachment(SceneRoot);
	BottomInteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BottomInteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	BottomInteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BottomInteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BottomInteractionPoint"));
	BottomInteractionPoint->SetupAttachment(SceneRoot);

	BottomInteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BottomInteractionWidget"));
	BottomInteractionWidget->SetupAttachment(BottomInteractionPoint);

	// TOP ZONE SETUP
	TopInteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TopInteractionBox"));
	TopInteractionBox->SetupAttachment(SceneRoot);
	TopInteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TopInteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TopInteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TopInteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TopInteractionPoint"));
	TopInteractionPoint->SetupAttachment(SceneRoot);

	TopInteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TopInteractionWidget"));
	TopInteractionWidget->SetupAttachment(TopInteractionPoint);

}

void ALadder::Interact(AProtagonist* Protagonist)
{
	BottomInteractionWidget->SetVisibility(false);
	TopInteractionWidget->SetVisibility(false);
	Protagonist->BeginLadderInteraction(this);
}


void ALadder::BeginPlay()
{
	Super::BeginPlay();

	BottomInteractionWidget->SetVisibility(false);
	TopInteractionWidget->SetVisibility(false);

	BottomInteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnBottomBoxOverlap);
	BottomInteractionBox->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnBottomBoxEndOverlap);

	TopInteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnTopBoxOverlap);
	TopInteractionBox->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnTopBoxEndOverlap);
	
}

void ALadder::OnBottomBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	if (Protagonist)
	{
		if (Protagonist->GetActionState() == EActionState::EAS_LadderClimbing ||
			Protagonist->GetActionState() == EActionState::EAS_LadderEntering ||
			Protagonist->GetActionState() == EActionState::EAS_LadderExiting)
		{
			return;
		}

		Protagonist->SetCurrentInteractable(this);
		bIsPlayerAtTop = false; // Player is at the bottom

		if (UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(BottomInteractionWidget->GetUserWidgetObject()))
		{
			PromptWidget->SetInteractionType(InteractionType);
		}
		BottomInteractionWidget->SetVisibility(true);
	}
}

void ALadder::OnBottomBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	if (Protagonist)
	{
		if (Protagonist->GetCurrentInteractable() == this)
		{
			Protagonist->SetCurrentInteractable(nullptr);
		}
		BottomInteractionWidget->SetVisibility(false);
	}
}

void ALadder::OnTopBoxOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (Protagonist)
	{
		if (Protagonist->GetActionState() == EActionState::EAS_LadderClimbing ||
			Protagonist->GetActionState() == EActionState::EAS_LadderEntering ||
			Protagonist->GetActionState() == EActionState::EAS_LadderExiting)
		{
			return;
		}

		Protagonist->SetCurrentInteractable(this);
		bIsPlayerAtTop = true; // Player is at the top

		if (UInteractionPromptWidget* PromptWidget = Cast<UInteractionPromptWidget>(TopInteractionWidget->GetUserWidgetObject()))
		{
			PromptWidget->SetInteractionType(InteractionType);
		}
		TopInteractionWidget->SetVisibility(true);
	}
}

void ALadder::OnTopBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	if (Protagonist)
	{
		if (Protagonist->GetCurrentInteractable() == this)
		{
			Protagonist->SetCurrentInteractable(nullptr);
		}
		TopInteractionWidget->SetVisibility(false);
	}
}

FVector ALadder::GetBottomInteractionLocation() const { return BottomInteractionPoint->GetComponentLocation(); }
FRotator ALadder::GetBottomInteractionRotation() const { return BottomInteractionPoint->GetComponentRotation(); }
FVector ALadder::GetTopInteractionLocation() const { return TopInteractionPoint->GetComponentLocation(); }
FRotator ALadder::GetTopInteractionRotation() const { return TopInteractionPoint->GetComponentRotation(); }


