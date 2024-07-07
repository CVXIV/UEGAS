// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* Instigator, AActor* Target) const {
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.Instigator = Instigator;
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent =  GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	for (const TTuple<FGameplayTag, FDamageTypeInfo>& Pair : DamageTypes) {
		Params.DamageTypesKeys.Add(Pair.Key);
		Params.DamageTypesValues.Add(Pair.Value);
	}
	Params.AbilityLevel = GetAbilityLevel();
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude * 1000.f;
	Params.KnockBackForceMagnitude = KnockBackForceMagnitude * 1000.f;
	Params.bTakeHitReact = bTakeHitReact;

	return Params;
}
