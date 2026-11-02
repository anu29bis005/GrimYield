
#include "HUD/CreditsWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UCreditsWidget::PlayCreditsSequence()
{
	if (CreditsRollAnimation)
	{
		PlayAnimation(CreditsRollAnimation);
	}

	if (CreditsMusic)
	{
		UGameplayStatics::PlaySound2D(this, CreditsMusic);
	}

	SetIsFocusable(true);

	GetWorld()->GetTimerManager().SetTimer(
		SkipDelayTimer,
		this,
		&UCreditsWidget::EnableSkipping,
		14.0f,
		false
	);
}

void UCreditsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsHoldingSkip && !bHasQuit)
	{
		CurrentHoldTime += InDeltaTime;

		if (CurrentHoldTime >= RequiredHoldTime)
		{
			bHasQuit = true;
			OnCreditsFinished();
		}
	}
}

FReply UCreditsWidget::NativeOnKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent)
{
	if (bCanSkip && InKeyEvent.GetKey() == EKeys::Q)
	{
		bIsHoldingSkip = true;
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UCreditsWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Q)
	{
		bIsHoldingSkip = false;
		CurrentHoldTime = 0.f;
		return FReply::Handled();
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UCreditsWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == CreditsRollAnimation && !bHasQuit)
	{
		bHasQuit = true;
		OnCreditsFinished();
	}
}

void UCreditsWidget::OnCreditsFinished()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(
			GetWorld(),
			PC,
			EQuitPreference::Quit,
			false
		);
	}
}

void UCreditsWidget::EnableSkipping()
{
	bCanSkip = true;
}
