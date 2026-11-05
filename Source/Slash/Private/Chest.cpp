
#include "Chest.h"
#include "HUD/InteractionPromptWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/Protagonist.h"
#include "Items/Treasure.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

AChest::AChest()
{

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	//Chest Base
	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(SceneRoot);

	//Chest Lid
	ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLid"));
	ChestLid->SetupAttachment(ChestMesh);

	//Box Collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	BoxCollision->SetupAttachment(SceneRoot);

	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnBoxOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AChest::OnBoxEndOverlap);

	// Interaction Point
	InteractionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionPoint"));
	InteractionPoint->SetupAttachment(SceneRoot);

	// Treasure Spawn Point
	TreasureSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TreasureSpawnPoint"));
	TreasureSpawnPoint->SetupAttachment(SceneRoot);

	// Interaction Widget
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(InteractionPoint);
	HideInteractionWidget();

}

void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLidAnimating)
	{
		UpdateLidAnimation(DeltaTime);
	}

	if (bFadingOut)
	{
		UpdateFade(DeltaTime);
	}

	if (!bLidAnimating && !bFadingOut)
	{
		SetActorTickEnabled(false);
	}
}

void AChest::Interact(AProtagonist* Protagonist)
{
	if (ChestState != EChestState::ECS_Closed)
	{
		return;
	}

	if (RequiredKey != EKeyType::EKT_None && !Protagonist->HasKey(RequiredKey))
	{
		return;
	}

	ChestState = EChestState::ECS_Opening;
	HideInteractionWidget();

	Protagonist->BeginChestInteraction(this);
	UE_LOG(LogTemp, Warning, TEXT("Chest Interaction Started"));
}

void AChest::OpenLidPhaseOne()
{
	TargetLidRoll = -60.f;
	bLidAnimating = true;

	SetActorTickEnabled(true);

	if (ChestOpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ChestOpenSound,
			GetActorLocation(),
			FRotator::ZeroRotator,
			1.f,
			1.f,
			OpenSoundStartTime
		);
	}
}

void AChest::OpenLidPhaseTwo()
{
	TargetLidRoll = -120.f;
	bLidAnimating = true;

	SetActorTickEnabled(true);
}

void AChest::CancelOpening(AProtagonist* Protagonist)
{
	if (ChestState != EChestState::ECS_Opening)
	{
		return;
	}

	bLidAnimating = false;
	TargetLidRoll = ClosedLidRotation.Roll;

	ChestLid->SetRelativeRotation(ClosedLidRotation);

	ChestState = EChestState::ECS_Closed;

	if (Protagonist && Protagonist->GetCurrentInteractable() == this)
	{
		ShowInteractionWidget();
	}

	UE_LOG(LogTemp, Warning, TEXT("Chest opening cancelled and reset"));
}


void AChest::SpawnTreasure()
{
	if (bTreasureSpawned)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (WeaponClassToSpawn)
	{
		FRotator FinalWeaponRotation = TreasureSpawnPoint->
			GetComponentRotation() + WeaponSpawnRotationOffset;

		AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(
			WeaponClassToSpawn,
			TreasureSpawnPoint->GetComponentLocation(),
			FinalWeaponRotation
		);

		if (SpawnedWeapon)
		{
			ChestState = EChestState::ECS_Opened;
			bTreasureSpawned = true;
		}

		return;
	}

	if (TreasureClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no Treasure Classes assigned."), *GetName());
		return;
	}

	const int32 RandomIndex = FMath::RandRange(0, TreasureClasses.Num() - 1);

	if (World && TreasureClasses[RandomIndex])
	{
		ATreasure* SpawnedTreasure = World->SpawnActor<ATreasure>(
			TreasureClasses[RandomIndex],
			TreasureSpawnPoint->GetComponentLocation(),
			FRotator::ZeroRotator);

		if (SpawnedTreasure)
		{
			SpawnedTreasure->SetOwningChest(this);
			ChestState = EChestState::ECS_Opened;
			bTreasureSpawned = true;
		}
	}
	
}

void AChest::OnTreasureCollected()
{
	ChestState = EChestState::ECS_LootCollected;

	ChestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChestLid->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bFadingOut = true;
	SetActorTickEnabled(true);
}


void AChest::BeginPlay()
{
	Super::BeginPlay();

	ClosedLidRotation = ChestLid->GetRelativeRotation();

	ChestMaterial = ChestMesh->CreateDynamicMaterialInstance(0);
	LidMaterial = ChestLid->CreateDynamicMaterialInstance(0);

	if (ChestMaterial && LidMaterial)
	{
		ChestMaterial->SetScalarParameterValue(TEXT("Fade"), 1.f);
		LidMaterial->SetScalarParameterValue(TEXT("Fade"), 1.f);
	}
	
}

void AChest::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	if (Protagonist)
	{
		if (RequiredKey != EKeyType::EKT_None && !Protagonist->HasKey(RequiredKey))
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
		}
		ShowInteractionWidget();
	}
}

void AChest::OnBoxEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AProtagonist* Protagonist = Cast<AProtagonist>(OtherActor);
	if (Protagonist)
	{
		if (Protagonist->GetCurrentInteractable() == this)
		{
			Protagonist->SetCurrentInteractable(nullptr);
		}
		HideInteractionWidget();
	}
}

void AChest::ShowInteractionWidget()
{
	if (ChestState == EChestState::ECS_Closed)
	{
		InteractionWidget->SetVisibility(true);
	}
}

void AChest::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}

void AChest::UpdateLidAnimation(float DeltaTime)
{
	FRotator Rotation = ChestLid->GetRelativeRotation();

	Rotation.Roll = FMath::FInterpTo(
		Rotation.Roll,
		TargetLidRoll,
		DeltaTime,
		LidInterpSpeed);

	if (FMath::IsNearlyEqual(Rotation.Roll, TargetLidRoll, 0.5f))
	{
		Rotation.Roll = TargetLidRoll;
		bLidAnimating = false;
	}

	ChestLid->SetRelativeRotation(Rotation);
}

void AChest::UpdateFade(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Fade = %f"), CurrentFade);
	CurrentFade = FMath::FInterpTo(
		CurrentFade,
		0.f,
		DeltaTime,
		FadeInterpSpeed);

	if (FMath::IsNearlyEqual(CurrentFade, 0.f, 0.05f))
	{
		CurrentFade = 0.f;
		bFadingOut = false;
	}
	if (ChestMaterial && LidMaterial)
	{
		ChestMaterial->SetScalarParameterValue(TEXT("Fade"), CurrentFade);
		LidMaterial->SetScalarParameterValue(TEXT("Fade"), CurrentFade);
	}
	if (!bFadingOut)
	{
		DestroyChest();
	}
}

void AChest::DestroyChest()
{
	UE_LOG(LogTemp, Warning, TEXT("DestroyChest called"));

	Destroy();
}

// Getting Location and Rotation of InteractionPoint
FVector AChest::GetInteractionLocation() const
{
	return InteractionPoint->GetComponentLocation();
}

FRotator AChest::GetInteractionRotation() const
{
	return InteractionPoint->GetComponentRotation();
}
