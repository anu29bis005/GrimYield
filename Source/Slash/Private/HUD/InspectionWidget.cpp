
#include "HUD/InspectionWidget.h"
#include "Components/Image.h"

void UInspectionWidget::SetInspectionTexture(UTexture2D* Texture)
{
	if (InspectionImage && Texture)
	{
		InspectionImage->SetBrushFromTexture(Texture);
	}
}
