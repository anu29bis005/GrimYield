// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{

	PrimaryComponentTick.bCanEverTick = false;


}



void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();


	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::isAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::AddValue(int32 AmountOfGold)
{
	Value += AmountOfGold;
}

void UAttributeComponent::AddSouls(int32 NumOfSouls)
{
	Souls += NumOfSouls;
}

void UAttributeComponent::AddHealthPotion(int32 NumOfHealthPotions)
{
	HealthPotions += NumOfHealthPotions;
}

bool UAttributeComponent::CanUseHealthPotion() const
{
	return HealthPotions > 0 && Health < MaxHealth;
}

bool UAttributeComponent::UseHealthPotion()
{
	if (!CanUseHealthPotion())
	{
		return false;
	}

	const float HealAmount = MaxHealth * HealthPotionHealPercent;

	Health = FMath::Clamp(
		Health + HealAmount,
		0.f,
		MaxHealth);

	HealthPotions--;

	return true;
}



void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

