// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE(FOnGameplayTagsInitialized);

struct FAuraGameplayTags {
	const static FAuraGameplayTags& Get() { return S_GameplayTags; }

	static void InitializeNativeGameplayTags();

	FGameplayTag None;

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

	FGameplayTag InputTag_Passive_1;

	FGameplayTag InputTag_Passive_2;

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

	FGameplayTag Montage_Attack_Tail;

	/**/

	/** Ability */
	FGameplayTag Ability_Attack;

	FGameplayTag Ability_Summon;

	FGameplayTag Ability_FireBolt;

	FGameplayTag Ability_Lightning_Electrocute;

	FGameplayTag Ability_Status_Locked;

	FGameplayTag Ability_Status_Eligible;

	FGameplayTag Ability_Status_Unlocked;

	FGameplayTag Ability_Status_Equipped;

	FGameplayTag Ability_Type_None;

	FGameplayTag Ability_Type_Offensive;

	FGameplayTag Ability_Type_Passive;

	/**/

	/** Cooldown */
	FGameplayTag Cooldown_FireBolt;

	/**/

	/** Meta*/
	FGameplayTag Attributes_Meta_IncomingXP;

	/**/

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistance;

	static FOnGameplayTagsInitialized S_OnGameplayTagsInitialized;

private:
	static FAuraGameplayTags S_GameplayTags;
};
