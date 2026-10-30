
#include "HUD/EnemyDefeatProgressionWidget.h"
#include "Components/TextBlock.h"


void UEnemyDefeatProgressionWidget::SetEnemiesRemaining(int32 NewCount)
{
	if (EnemyCount)
	{
		EnemyCount->SetText(FText::AsNumber(NewCount));
	}
}

void UEnemyDefeatProgressionWidget::SetClearanceText(const FText& NewClearanceText)
{
	if (ClearanceText)
	{
		ClearanceText->SetText(NewClearanceText);
	}
}
