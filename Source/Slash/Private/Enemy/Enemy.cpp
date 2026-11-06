// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Characters/Protagonist.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Items/KeyItem.h"
#include "GrimYieldGameInstance.h"


/** PUBLIC FUNCTIONS */

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWdgt = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWdgt->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

}

void AEnemy::SetEnemyCinematicHidden(bool bIsHidden)
{
	if (isDead())
	{
		return;
	}

	if (bIsHidden)
	{
		ClearPatrolTimer();
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		if (EquippedWeapon)
		{
			EquippedWeapon->SetActorHiddenInGame(true);
		}
	}

	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		if (EquippedWeapon)
		{
			EquippedWeapon->SetActorHiddenInGame(false);
		}
		StartPatrolling();
	}
}

void AEnemy::SetDormant(bool bDormant)
{
	bIsDormant = bDormant;

	if (bIsDormant)
	{
		ClearPatrolTimer();
		ClearAttackTimer();
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}
	}

	else
	{
		if (AProtagonist* Player = Cast<AProtagonist>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			CombatTarget = Player;
			ChaseTarget();
		}
		else
		{
			StartPatrolling();
		}
	}
}

void AEnemy::SetQuestOutline(bool bEnable)
{
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(bEnable);
		GetMesh()->SetCustomDepthStencilValue(1);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isDead() || bIsDormant) 
		return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (isDead())
	{
		return 0.f;
	}

	if (UGrimYieldGameInstance* GI = Cast<UGrimYieldGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (!GI->bIsHardMode)
		{
			DamageAmount *= 2.5f;
		}
	}

	if (bIsQuestTarget)
		SetQuestOutline(true);

	if (bIsDormant)
	{
		SetDormant(false);
	}

	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (isDead())
	{
		return DamageAmount;
	}

	if (isInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (isOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void AEnemy::Destroyed() // here we can implement to drop the weapon if we plan to do so in future
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (isDead())
	{
		return;
	}

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!isDead()) 
		ShowHealthBar();	
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision); 

	StopAttackMontage();
	if (isInsideAttackRadius())
	{
		if (!isDead())	
			StartAttackTimer();
	}
}

/** PROTECTED FUNCTIONS */

void AEnemy::HandleCinematicState(bool bIsCinematic)
{
	if (isDead())
	{
		return;
	}

	if (bIsCinematic)
	{
		CombatTarget = nullptr;
		ClearAttackTimer();
		StopAttackMontage();
		LoseInterest();
		StartPatrolling();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetMesh()->CanCharacterStepUpOn = ECB_No;

	if (PawnSensing)	
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));

	if (AProtagonist* Player = Cast<AProtagonist>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->OnCinematicStateToggled.AddDynamic(
			this, 
			&AEnemy::HandleCinematicState
		);
	}
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	EnemyState = EEnemyState::EES_Dead;
	
	OnEnemyDeath.Broadcast();

	SetQuestOutline(false);

	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
	SpawnKeyItem();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnSoul)
		{
			SpawnSoul->SetSouls(Attributes->GetSouls());
			SpawnSoul->SetOwner(this);
		}
			
	}
}

void AEnemy::SpawnKeyItem()
{
	UWorld* World = GetWorld();

	if (World && KeyItemClass && DroppedKeyType != EKeyType::EKT_None)
	{
		const FVector SpawnLocation =
			GetActorLocation() + FVector(0.f, 0.f, 50.f);

		AKeyItem* SpawnedKey = World->SpawnActor<AKeyItem>(
			KeyItemClass,
			SpawnLocation,
			GetActorRotation()
		);

		if (SpawnedKey)
		{
			SpawnedKey->SetKeyType(DroppedKeyType);
		}
	}
}

void AEnemy::AttackAction()
{	
	Super::AttackAction();
	if (CombatTarget == nullptr) 
		return;
	EnemyState = EEnemyState::EES_Engaged;

	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		isInsideAttackRadius() &&
		!isAttacking() &&
		!isEngaged() &&
		!isDead();
	return bCanAttack;
}

void AEnemy::EndAttack()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (Attributes && HealthBarWdgt)
	{
		HealthBarWdgt->SetHealthPercent(Attributes->GetHealthPercent());
	}

}


/** PRIVATE FUNCTIONS */

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());

	HideHealthBar();
	SpawnDefaultWeapon();

	if (!bIsDormant)
	{
		MoveToTarget(CurrentPatrolTarget);
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(CurrentPatrolTarget, PatrolRadius))
	{
		CurrentPatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (isOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!isEngaged())	StartPatrolling();
	}

	else if (isOutsideAttackRadius() && !isChasing())
	{
		ClearAttackTimer();
		if (!isEngaged())	ChaseTarget();
	}

	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(CurrentPatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWdgt)
	{
		HealthBarWdgt->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWdgt)
	{
		HealthBarWdgt->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	if (bIsQuestTarget)
		SetQuestOutline(false);

	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = 125.f;
	MoveToTarget(CurrentPatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackTimeMin, AttackTimeMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::AttackAction, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)	return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

bool AEnemy::isOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::isOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::isInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::isDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::isChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::isAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::isEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != CurrentPatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DeafaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DeafaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DeafaultWeapon;
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (bIsDormant)
	{
		return;
	}

	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));
	if (bShouldChaseTarget)
	{
		if (bIsQuestTarget) 
			SetQuestOutline(true);

		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

