
#include "HUD/GameOver.h"
#include "HUD/GameHUD.h"
#include "GameFramework/PlayerController.h"
#include "Animation/WidgetAnimation.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameOver::ShowGameOver(float Delay)
{

		if (Delay <= 0.0f)
		{
			PlayAnimation(GameOverAnimation);
			PlaySound(GameOverSound);
			SetGameOverInput();
			StartFreezeTimer();

			return;
		}

		FTimerHandle GameOverTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			GameOverTimerHandle,
			[this]()
			{
				PlayAnimation(GameOverAnimation);
				PlaySound(GameOverSound);
				SetGameOverInput();
				StartFreezeTimer();
			},
			Delay,
			false
		);

}

void UGameOver::SetGameOverInput()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (!PlayerController)
	{
		return;
	}

	if (AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		GameHUD->SetObjectiveTrackerSuppressed(true);
	}

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	
}

void UGameOver::StartFreezeTimer()
{
	FTimerHandle FreezeTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		FreezeTimerHandle,
		[this]()
		{
			FreezeGame();
		},
		1.0f,
		false
	);
}

void UGameOver::RestartGame()
{
	UGameplayStatics::OpenLevel(
		GetWorld(),
		FName(*GetWorld()->GetName())
	);
}

void UGameOver::QuitGame()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(
			GetWorld(),
			PlayerController,
			EQuitPreference::Quit,
			false
		);
	}
}

void UGameOver::FreezeGame()
{
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}
