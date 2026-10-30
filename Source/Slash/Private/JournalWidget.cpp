
#include "JournalWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"

void UJournalWidget::OpenJournal(const TArray<UTexture2D*>& InNotes)
{
	CollectedNotes = InNotes;
	CurrentIndex = 0;
	UpdateDisplay();
}

void UJournalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LeftButton)
	{
		LeftButton->OnClicked.AddDynamic(
			this,
			&UJournalWidget::OnLeftButtonClicked
		);
	}

	if (RightButton)
	{
		RightButton->OnClicked.AddDynamic(
			this,
			&UJournalWidget::OnRightButtonClicked
		);
	}
}

void UJournalWidget::OnLeftButtonClicked()
{
	if (CurrentIndex > 0)
	{
		CurrentIndex--;
		UpdateDisplay();
	}
}

void UJournalWidget::OnRightButtonClicked()
{
	if (CurrentIndex < CollectedNotes.Num() - 1)
	{
		CurrentIndex++;
		UpdateDisplay();
	}
}

void UJournalWidget::UpdateDisplay()
{
	if (CollectedNotes.IsValidIndex(CurrentIndex) && NoteImage)
	{
		NoteImage->SetBrushFromTexture(CollectedNotes[CurrentIndex]);
	}

	if (LeftButton)
	{
		LeftButton->SetVisibility(
			CurrentIndex > 0 ? 
			ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	if (RightButton)
	{
		RightButton->SetVisibility(
			CurrentIndex < CollectedNotes.Num() - 1 ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
}
