#pragma once

#include "EnvironmentTypes.generated.h"

UENUM(BlueprintType)
enum class EChestState : uint8
{
	ECS_Closed			UMETA(DisplayName = "Closed"),
	ECS_Opening			UMETA(DisplayName = "Opening"),
	ECS_Opened			UMETA(DisplayName = "Opened"),
	ECS_LootCollected	UMETA(DisplayName = "Loot Collected")
};

UENUM(BlueprintType)
enum class EGateState : uint8
{
	EGS_Locked			UMETA(DisplayName = "Locked"),
	EGS_Unlocked		UMETA(DisplayName = "Unlocked")
};

UENUM(BlueprintType)
enum class ELeverState : uint8
{
	ELS_Negative90		UMETA(DisplayName = "Negative90"),
	ELS_Zero			UMETA(DisplayName = "Zero"),
	ELS_Positive90		UMETA(DisplayName = "Positive90")
};

UENUM(BlueprintType)
enum class ELeverSide : uint8
{
	ELS_LeftWall		UMETA(DisplayName = "Left Wall"),
	ELS_RightWall		UMETA(DisplayName = "Right Wall")
};

UENUM(BlueprintType)
enum class EKeyType : uint8
{
	EKT_GoldKey			UMETA(DisplayName = "Gold Key"),
	EKT_SilverKey		UMETA(DisplayName = "Silver Key"),
	EKT_RustedKey		UMETA(DisplayName = "Rusted Key"),
	EKT_None			UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EPushState : uint8
{
	EPS_Idle,
	EPS_InitialPush,
	EPS_ManualPush,
	EPS_FinalPush,
	EPS_Finished
};