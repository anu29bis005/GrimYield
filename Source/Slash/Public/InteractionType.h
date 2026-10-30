#pragma once

#include "InteractionType.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	EIT_Open	UMETA(DisplayName = "Open"),
	EIT_Use		UMETA(DisplayName = "Use"),
	EIT_Push	UMETA(DisplayName = "Push"),
	EIT_Pull	UMETA(DisplayName = "Pull"),
	EIT_Pickup	UMETA(DisplayName = "Pickup"),
	EIT_Talk	UMETA(DisplayName = "Talk"),
	EIT_Inspect	UMETA(DisplayName = "Inspect"),
	EIT_Read	UMETA(DisplayName = "Read")
};