// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FAuraGameplayTags {
public:
	const static FAuraGameplayTags& Get() { return S_GameplayTags; }

	static void InitializeNativeGameplayTags();

	FGameplayTag Attribute_Primary_Strength;

	FGameplayTag Attribute_Primary_Intelligence;

	FGameplayTag Attribute_Primary_Resilience;

	FGameplayTag Attribute_Primary_Vigor;

	FGameplayTag Attribute_Secondary_Armor;

	FGameplayTag Attribute_Secondary_ArmorPenetration;

	FGameplayTag Attribute_Secondary_BlockChance;

	FGameplayTag Attribute_Secondary_CriticalHitChance;

	FGameplayTag Attribute_Secondary_CriticalHitDamage;

	FGameplayTag Attribute_Secondary_CriticalHitResistance;

	FGameplayTag Attribute_Secondary_HealthRegeneration;

	FGameplayTag Attribute_Secondary_ManaRegeneration;

	FGameplayTag Attribute_Secondary_MaxHealth;

	FGameplayTag Attribute_Secondary_MaxMana;

	// Input Tag
	FGameplayTag InputTag_LMB;

	FGameplayTag InputTag_RMB;

	FGameplayTag InputTag_1;

	FGameplayTag InputTag_2;

	FGameplayTag InputTag_3;

	FGameplayTag InputTag_4;

protected:
private:
	static FAuraGameplayTags S_GameplayTags;
};
