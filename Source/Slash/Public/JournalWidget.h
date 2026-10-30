// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JournalWidget.generated.h"

class UImage;
class UButton;
class UTexture2D;

UCLASS()
class SLASH_API UJournalWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void OpenJournal(const TArray<UTexture2D*>& InNotes);

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* NoteImage;

	UPROPERTY(meta = (BindWidget))
	UButton* LeftButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RightButton;

private:

	UFUNCTION()
	void OnLeftButtonClicked();

	UFUNCTION()
	void OnRightButtonClicked();

	void UpdateDisplay();

	UPROPERTY()
	TArray<UTexture2D*> CollectedNotes;

	int32 CurrentIndex = 0;
};
