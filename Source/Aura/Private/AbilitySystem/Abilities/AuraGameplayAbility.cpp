// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level) const {
	return FString::Printf(TEXT("<Default>%s,%d</>"), L"Default Ability Description", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level) const {
	return FString::Printf(TEXT("<Default>Next Level:%d,Causes much more damage</>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level) {
	return FString::Printf(TEXT("<Default>Spell Locked Until Level:%d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(int32 Level) const {
	float ManaCost = 0.0f;
	if (const UGameplayEffect* GE_Cost = GetCostGameplayEffect()) {
		for (const FGameplayModifierInfo& ModifierInfo : GE_Cost->Modifiers) {
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute()) {
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(int32 Level) const {
	float Cooldown = 0.0f;
	if (const UGameplayEffect* GE_Cooldown = GetCooldownGameplayEffect()) {
		GE_Cooldown->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	}
	return Cooldown;
}
