
#include "Interactables/LetterNote.h"

#include "Characters/Protagonist.h"
#include "HUD/InteractionPromptWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

ALetterNote::ALetterNote()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	NoteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NoteMesh"));
	NoteMesh->SetupAttachment(Root);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Root);
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(Root);
	InteractionWidget->SetVisibility(false);

}


void ALetterNote::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ALetterNote::OnBoxBeginOverlap);

	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ALetterNote::OnBoxEndOverlap);

	if (InteractionWidget)
	{
		if (UInteractionPromptWidget* PromptWidget =
			Cast<UInteractionPromptWidget>(
				InteractionWidget->GetUserWidgetObject()))
		{
			PromptWidget->SetInteractionType(InteractionType);
		}
	}
}

void ALetterNote::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (Protagonist)
	{
		Protagonist->SetCurrentInteractable(this);

		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(true);
		}
	}
}

void ALetterNote::OnBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
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
}

void ALetterNote::Interact(AProtagonist* Protagonist)
{
	if (!Protagonist || !InspectionTexture)
	{
		return;
	}

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}

	Protagonist->BeginInspection(InspectionTexture, this);

}

void ALetterNote::RefreshInteractionPrompt(AProtagonist* Protagonist)
{
	if (!Protagonist)
	{
		return;
	}

	if (InteractionBox &&
		InteractionBox->IsOverlappingActor(Protagonist))
	{
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(true);
		}
	}
}

