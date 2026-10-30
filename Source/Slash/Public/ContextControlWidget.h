// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContextControlWidget.generated.h"


UCLASS()
class SLASH_API UContextControlWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetInspectionMode();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeverMode();

	UFUNCTION(BlueprintImplementableEvent)
	void SetOldMachineMode();

	UFUNCTION(BlueprintImplementableEvent)
	void SetCratePushMode();
};
