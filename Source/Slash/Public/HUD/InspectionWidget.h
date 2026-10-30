#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InspectionWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class SLASH_API UInspectionWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void SetInspectionTexture(UTexture2D* Texture);

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* InspectionImage;
};
