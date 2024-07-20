// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* Instigator, AActor* Target, FVector InRadialDamageOrigin) const {
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.Instigator = Instigator;
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	for (const TTuple<FGameplayTag, FDamageTypeInfo>& Pair : DamageTypes) {
		Params.DamageTypesKeys.Add(Pair.Key);
		Params.DamageTypesValues.Add(Pair.Value);
	}
	Params.AbilityLevel = GetAbilityLevel();
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude * 1000.f;
	Params.KnockBackForceMagnitude = KnockBackForceMagnitude * 1000.f;
	Params.bTakeHitReact = bTakeHitReact;

	if (bIsRadialDamage) {
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadiusDamageOuterRadius = RadiusDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
	}

	return Params;
}

FString UAuraDamageGameplayAbility::GetDescription(int32 Level) const {
	FString DamageStr;
	for (auto& Pair : DamageTypes) {
		const int Damage = Pair.Value.Damage.GetValueAtLevel(Level);
		if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Fire)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Fire Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Lightning)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Lightning Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Arcane)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Arcane Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Physical)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Physical Damage!</>\n"), Damage));
		}
	}
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Level>LEVEL:%d</>\n\n<Small>ManaCost:%.1f</>\n\n<Small>Cooldown:%.1f</>\n\n<Default>Cause:</>\n%s"), Level, ManaCost, Cooldown, *DamageStr);
}

FString UAuraDamageGameplayAbility::GetNextLevelDescription(int32 Level) const {
	return GetDescription(Level + 1);
}
