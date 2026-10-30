
#include "Characters/GrimNPC.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/InteractionPromptWidget.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "Characters/Protagonist.h"
#include "Animation/AnimInstance.h"

AGrimNPC::AGrimNPC()
{
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(GetMesh());

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetMesh());

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());

	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetupAttachment(GetMesh());

	ShirtMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShirtMesh"));
	ShirtMesh->SetupAttachment(GetMesh());
	ShirtMesh->SetHiddenInGame(true);

	ApronMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ApronMesh"));
	ApronMesh->SetupAttachment(GetMesh());
	ApronMesh->SetHiddenInGame(true);

	TrousersMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TrousersMesh"));
	TrousersMesh->SetupAttachment(GetMesh());
	TrousersMesh->SetHiddenInGame(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AGrimNPC::OnBoxOverlap
	);
	BoxCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&AGrimNPC::OnBoxEndOverlap
	);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	InteractionWidget->SetVisibility(false);

	CryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CryAudioComponent"));
	CryAudioComponent->SetupAttachment(RootComponent);
	CryAudioComponent->bAutoActivate = false;

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AGrimNPC::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (HairMesh) HairMesh->SetLeaderPoseComponent(GetMesh());
	if (HeadMesh) HeadMesh->SetLeaderPoseComponent(GetMesh());
	if (HandsMesh) HandsMesh->SetLeaderPoseComponent(GetMesh());
	if (FeetMesh) FeetMesh->SetLeaderPoseComponent(GetMesh());

	if (ShirtMesh) ShirtMesh->SetLeaderPoseComponent(GetMesh());
	if (ApronMesh) ApronMesh->SetLeaderPoseComponent(GetMesh());
	if (TrousersMesh) TrousersMesh->SetLeaderPoseComponent(GetMesh());
}

void AGrimNPC::Interact(AProtagonist* Protagonist)
{
	if (!bIsFreed || !Protagonist)
	{
		return;
	}

	InteractionWidget->SetVisibility(false);
	Protagonist->SetCurrentInteractable(nullptr);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayReleaseCutscene(Protagonist);
}

void AGrimNPC::SetFreedState(bool bFreed)
{
	bIsFreed = bFreed;
}

void AGrimNPC::StopCryingForHelp()
{
	// Killing the timer so the loop breaks
	GetWorldTimerManager().ClearTimer(CryTimerHandle);

	// Fading out the current cry over 1 second so it doesn't abruptly cut off
	if (CryAudioComponent && CryAudioComponent->IsPlaying())
	{
		CryAudioComponent->FadeOut(
			1.f,
			0.f
		);
	}
}

void AGrimNPC::BeginPlay()
{
	Super::BeginPlay();

	if (bIsCaptive)
	{
		GetWorldTimerManager().SetTimer(
			CryTimerHandle,
			this,
			&AGrimNPC::PlayCrySound,
			FMath::RandRange(MinCryInterval, MaxCryInterval),
			false
		);
	}
}

void AGrimNPC::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsFreed)
	{
		return;
	}

	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);

	if (Protagonist)
	{
		Protagonist->SetCurrentInteractable(this);

		if (InteractionWidget)
		{
			UInteractionPromptWidget* PromptWidget =
				Cast<UInteractionPromptWidget>(
					InteractionWidget->GetUserWidgetObject()
				);

			if (PromptWidget)
			{
				PromptWidget->SetInteractionType(InteractionType);
			}
			InteractionWidget->SetVisibility(true);
		}
	}
}

void AGrimNPC::OnBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!bIsFreed)
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

void AGrimNPC::PlayReleaseCutscene(AProtagonist* Protagonist)
{
	if (Protagonist)
	{
		Protagonist->BeginCinematicState();
	}

	if (WearingApparelMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(WearingApparelMontage);
	}

	TriggerCameraSequence();
}

void AGrimNPC::EquipApparel()
{
	if (ShirtMesh)	ShirtMesh->SetHiddenInGame(false);
	if (ApronMesh)	ApronMesh->SetHiddenInGame(false);
	if (TrousersMesh)	TrousersMesh->SetHiddenInGame(false);

	if (HandsMesh && GlovesMeshAsset)
	{
		HandsMesh->SetSkeletalMeshAsset(GlovesMeshAsset);
	}

	if (FeetMesh && BootsMeshAsset)
	{
		FeetMesh->SetSkeletalMeshAsset(BootsMeshAsset);
	}
}

void AGrimNPC::PlayCrySound()
{
	if (CryAudioComponent && CryAudioComponent->GetSound())
	{
		CryAudioComponent->Play();
	}

	// Recursively setting the timer again for a new random interval
	GetWorldTimerManager().SetTimer(
		CryTimerHandle,
		this,
		&AGrimNPC::PlayCrySound,
		FMath::RandRange(MinCryInterval, MaxCryInterval),
		false
	);
}
