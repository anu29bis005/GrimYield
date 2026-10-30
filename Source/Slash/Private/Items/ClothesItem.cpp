
#include "Items/ClothesItem.h"
#include "Interfaces/PickupInterface.h"

void AClothesItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		PickupInterface->AddClothes(this);
		ShowItemPickupNotification(OtherActor);
		SpawnPickupSound();
		Destroy();
	}
}
