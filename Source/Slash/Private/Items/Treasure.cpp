// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"
#include "Chest.h"

void ATreasure::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(
		EnableCollisionTimer,
		this,
		&ATreasure::EnablePickupCollision,
		1.f,
		false);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddValue(this);

		ShowItemPickupNotification(OtherActor);

		if (OwningChest)
		{
			OwningChest->OnTreasureCollected();
		}
		SpawnPickupSound();
		Destroy();
	}
}

void ATreasure::EnablePickupCollision()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
