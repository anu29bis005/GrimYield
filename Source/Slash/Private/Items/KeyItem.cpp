// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/KeyItem.h"
#include "Interfaces/PickupInterface.h"

void AKeyItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddKey(this);

		ShowItemPickupNotification(OtherActor);

		SpawnPickupSound();
		Destroy();
	}
}
