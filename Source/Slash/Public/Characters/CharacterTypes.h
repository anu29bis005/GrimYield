#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped				UMETA(DisplayName = "Unequipped"), 
	ECS_EquipOneHandedWeapon	UMETA(DisplayName = "EquipOneHandedWeapon"), 
	ECS_EquipTwoHandedWeapon	UMETA(DisplayName = "EquipTwoHandedWeapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied				UMETA(DisplayName = "Unoccupied"),

	EAS_LeverCameraTransition	UMETA(DisplayName = "LeverCameraTransition"),
	EAS_LeverInteraction		UMETA(DisplayName = "LeverInteraction"),
	EAS_LeverRotating			UMETA(DisplayName = "LeverRotating"),

	EAS_Pushing					UMETA(DisplayName = "Pushing"),

	EAS_MachineCameraTransition UMETA(DisplayName = "MachineCameraTransition"),
	EAS_MachineInteraction		UMETA(DisplayName = "MachineInteraction"),
	
	EAS_Cinematic				UMETA(DisplayName = "Cinematic"),
	EAS_HitReaction				UMETA(DisplayName = "HitReaction"), 
	EAS_Attacking				UMETA(DisplayName = "Attacking"), 
	EAS_EquippingWeapon			UMETA(DisplayName = "EquippingWeapon"), 
	EAS_Dodge					UMETA(DisplayName = "Dodge"),
	EAS_Drinking				UMETA(DisplayName = "Drinking"),

	EAS_LadderEntering			UMETA(DisplayName = "LadderEntering"),
	EAS_LadderClimbing			UMETA(DisplayName = "LadderClimbing"),
	EAS_LadderExiting			UMETA(DisplayName = "LadderExiting"),

	EAS_Dead					UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1					UMETA(DisplayName = "Death1"), 
	EDP_Death2					UMETA(DisplayName = "Death2"), 
	EDP_Death3					UMETA(DisplayName = "Death3"), 
	EDP_Death4					UMETA(DisplayName = "Death4"), 

	EDP_MAX						UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState					UMETA(DisplayName = "NoState"), 
	EES_Dead					UMETA(DisplayName = "Dead"), 
	EES_Patrolling				UMETA(DisplayName = "Patrolling"), 
	EES_Chasing					UMETA(DisplayName = "Chasing"), 
	EES_Attacking				UMETA(DisplayName = "Attacking"), 
	EES_Engaged					UMETA(DisplayName = "Engaged")
};

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	ENS_Idle					UMETA(DisplayName = "Idle")
};

