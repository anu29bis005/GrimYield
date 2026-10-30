// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool isAlive();
	void AddValue(int32 AmountOfGold);
	void AddSouls(int32 NumOfSouls);
	void AddHealthPotion(int32 NumOfHealthPotions);
	bool CanUseHealthPotion() const;
	bool UseHealthPotion();


protected:

	virtual void BeginPlay() override;

private:	

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float Health;	//Current Health

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float Stamina;	//Current Stamina

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	int32 Value;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	int32 HealthPotions = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float HealthPotionHealPercent = 0.30f;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	float StaminaRegenRate = 8.f;
		
public:

	FORCEINLINE int32 GetValue() const { return Value;	 }
	FORCEINLINE int32 GetSouls() const { return Souls;	}
	FORCEINLINE int32 GetHealthPotions() const { return HealthPotions; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
