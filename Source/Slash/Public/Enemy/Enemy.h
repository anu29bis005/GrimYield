// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "EnvironmentTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UHealthBarComponent;
class UPawnSensingComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);


UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy();

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnEnemyDeath OnEnemyDeath;

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	void SpawnKeyItem();
	virtual void AttackAction() override;
	virtual bool CanAttack() override;
	virtual void EndAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	/** <ABaseCharacter> */

	

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	
	/** <AI Behavior> */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	void MoveToTarget(AActor* Target);
	bool isOutsideCombatRadius();
	bool isOutsideAttackRadius();
	bool isInsideAttackRadius();
	bool isDead();
	bool isChasing();
	bool isAttacking();
	bool isEngaged();
	bool InTargetRange(AActor* Target, double Radius);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);	//Callback for OnPawnSeen in UPawnSensingComponent


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWdgt;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf <class AWeapon> WeaponClass;


	UPROPERTY(EditAnywhere, Category="Combat")
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 50.f;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* CurrentPatrolTarget;	// Current Patrol Target

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation");
	TArray<AActor*> PatrolTargets;		// Array of patrol targets

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "Combat");
	float PatrollingSpeed = 125.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat");
	float AttackTimeMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat");
	float AttackTimeMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat");
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	EKeyType DroppedKeyType = EKeyType::EKT_None;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AKeyItem> KeyItemClass;

};
