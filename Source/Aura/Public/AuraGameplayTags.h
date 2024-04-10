// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE(FOnGameplayTagsInitialized);

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

	// 属性抵抗
	FGameplayTag Attributes_Resistance_Fire;

	FGameplayTag Attributes_Resistance_Lightning;

	FGameplayTag Attributes_Resistance_Arcane;

	FGameplayTag Attributes_Resistance_Physical;

	// Input Tag
	FGameplayTag InputTag_LMB;

	FGameplayTag InputTag_RMB;

	FGameplayTag InputTag_1;

	FGameplayTag InputTag_2;

	FGameplayTag InputTag_3;

	FGameplayTag InputTag_4;

	FGameplayTag DurationHeal;

	// Combat
	FGameplayTag Damage;

	FGameplayTag Damage_Fire;

	FGameplayTag Damage_Lightning;

	FGameplayTag Damage_Arcane;

	FGameplayTag Damage_Physical;

	/** 同时只有一个行为的Tag*/
	FGameplayTag Action_HitReact;

	FGameplayTag Action_Attack;

	/**/
	
	/** Montage*/
	FGameplayTag Montage_Attack_Weapon;

	FGameplayTag Montage_Attack_RightHand;

	FGameplayTag Montage_Attack_LeftHand;

	/**/

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistance;

	static FOnGameplayTagsInitialized S_OnGameplayTagsInitialized;

protected:
private:
	static FAuraGameplayTags S_GameplayTags;
};
