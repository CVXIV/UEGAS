// Copyright Cvxiv


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::S_GameplayTags;

FOnGameplayTagsInitialized FAuraGameplayTags::S_OnGameplayTagsInitialized;

FDeBuffInfo::FDeBuffInfo(const FGameplayTag& DeBuff, const FGameplayTag& InResistanceTag) {
	this->DeBuffTag = DeBuff;
	this->ResistanceTag = InResistanceTag;
	this->DeBuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(*(DeBuff.GetTagName().ToString() + FAuraGameplayTags::Get().DeBuff_Chance_Name.ToString()));
	this->DeBuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(*(DeBuff.GetTagName().ToString() + FAuraGameplayTags::Get().DeBuff_Damage_Name.ToString()));
	this->DeBuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(*(DeBuff.GetTagName().ToString() + FAuraGameplayTags::Get().DeBuff_Duration_Name.ToString()));
	this->DeBuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(*(DeBuff.GetTagName().ToString() + FAuraGameplayTags::Get().DeBuff_Frequency_Name.ToString()));
}

void FAuraGameplayTags::InitializeNativeGameplayTags() {
	S_GameplayTags.None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.None"), FString("空"));
	/**
	 * Primary
	 */
	S_GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"), FString("力量"));
	S_GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"), FString("智力"));
	S_GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"), FString("韧性"));
	S_GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"), FString("活力"));

	/**
	 * Secondary
	 */
	S_GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("护甲"));
	S_GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("护甲穿透"));
	S_GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("格挡几率"));
	S_GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"), FString("暴击率"));
	S_GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"), FString("暴击伤害"));
	S_GameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"), FString("暴击格挡"));
	S_GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"), FString("生命再生"));
	S_GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"), FString("法力再生"));
	S_GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("最大生命"));
	S_GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"), FString("最大法力"));

	/**
	 * InputTag
	 */
	S_GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"), FString("左键"));
	S_GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"), FString("右键"));
	S_GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"), FString("1"));
	S_GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"), FString("2"));
	S_GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.3"), FString("3"));
	S_GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.4"), FString("4"));
	S_GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive.1"), FString("Passive1"));
	S_GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive.2"), FString("Passive2"));

	S_GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Damage"));

	S_GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Fire"), FString("Fire Type Damage"));
	S_GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Lightning"), FString("Lightning Type Damage"));
	S_GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Arcane"), FString("Arcane Type Damage"));
	S_GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Physical"), FString("Physical Type Damage"));
	S_GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Fire"), FString("Resistance To Fire Damage"));
	S_GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Lightning"), FString("Resistance To Lightning Damage"));
	S_GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Arcane"), FString("Resistance To Arcane Damage"));
	S_GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Physical"), FString("Resistance To Physical Damage"));

	S_GameplayTags.Action_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Action.HitReact"));
	S_GameplayTags.Action_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Action.Attack"));

	S_GameplayTags.DurationHeal = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("DurationHeal"));

	S_GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Weapon"));
	S_GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.LeftHand"));
	S_GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.RightHand"));
	S_GameplayTags.Montage_Attack_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Tail"));

	/** Ability */
	S_GameplayTags.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Attack"));
	S_GameplayTags.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Summon"));
	S_GameplayTags.Ability_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.FireBolt"));
	S_GameplayTags.Ability_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.FireBlast"));
	S_GameplayTags.Ability_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Lightning.Electrocute"));
	S_GameplayTags.Ability_Arcane_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Arcane.ArcaneShards"));
	S_GameplayTags.Ability_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Locked"));
	S_GameplayTags.Ability_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Eligible"));
	S_GameplayTags.Ability_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Unlocked"));
	S_GameplayTags.Ability_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Equipped"));
	S_GameplayTags.Ability_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.None"));
	S_GameplayTags.Ability_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Offensive"));
	S_GameplayTags.Ability_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive"));
	S_GameplayTags.Ability_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.HaloOfProtection"));
	S_GameplayTags.Ability_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.LifeSiphon"));
	S_GameplayTags.Ability_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.ManaSiphon"));


	S_GameplayTags.Cooldown_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.FireBolt"));
	S_GameplayTags.Cooldown_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.FireBlast"));
	S_GameplayTags.Cooldown_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.ArcaneShards"));

	S_GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Meta.IncomingXP"));

	/**DeBuff*/
	S_GameplayTags.DeBuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("DeBuff.Burn"));

	S_GameplayTags.DeBuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("DeBuff.Stun"));

	S_GameplayTags.DeBuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("DeBuff.Arcane"));

	S_GameplayTags.DeBuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("DeBuff.Physical"));

	S_GameplayTags.DeBuff_Chance_Name = FName(".Chance");

	S_GameplayTags.DeBuff_Damage_Name = FName(".Damage");

	S_GameplayTags.DeBuff_Frequency_Name = FName(".Frequency");

	S_GameplayTags.DeBuff_Duration_Name = FName(".Duration");

	/**GameplayCue*/
	S_GameplayTags.Cue_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Burn"));
	S_GameplayTags.Cue_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Stun"));
	S_GameplayTags.Cue_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.HaloOfProtection"));
	S_GameplayTags.Cue_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.LifeSiphon"));
	S_GameplayTags.Cue_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.ManaSiphon"));
	S_GameplayTags.Cue_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.ArcaneShards"));

	// Map
	S_GameplayTags.DamageTypesToDeBuffAndResistance.Add(S_GameplayTags.Damage_Fire, FDeBuffInfo(S_GameplayTags.DeBuff_Burn, S_GameplayTags.Attributes_Resistance_Fire));
	S_GameplayTags.DamageTypesToDeBuffAndResistance.Add(S_GameplayTags.Damage_Lightning, FDeBuffInfo(S_GameplayTags.DeBuff_Stun, S_GameplayTags.Attributes_Resistance_Lightning));
	S_GameplayTags.DamageTypesToDeBuffAndResistance.Add(S_GameplayTags.Damage_Arcane, FDeBuffInfo(S_GameplayTags.DeBuff_Arcane, S_GameplayTags.Attributes_Resistance_Arcane));
	S_GameplayTags.DamageTypesToDeBuffAndResistance.Add(S_GameplayTags.Damage_Physical, FDeBuffInfo(S_GameplayTags.DeBuff_Physical, S_GameplayTags.Attributes_Resistance_Physical));

	S_GameplayTags.PassiveAbilityToCue.Add(S_GameplayTags.Ability_Passive_HaloOfProtection, S_GameplayTags.Cue_HaloOfProtection);
	S_GameplayTags.PassiveAbilityToCue.Add(S_GameplayTags.Ability_Passive_LifeSiphon, S_GameplayTags.Cue_LifeSiphon);
	S_GameplayTags.PassiveAbilityToCue.Add(S_GameplayTags.Ability_Passive_ManaSiphon, S_GameplayTags.Cue_ManaSiphon);

	S_GameplayTags.S_OnGameplayTagsInitialized.Broadcast();
}
