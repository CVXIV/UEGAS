// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemComponent.h"

TMap<TSubclassOf<UGameplayEffect>, int32>& UAuraAbilitySystemComponent::GetGameplayEffectDenyCount() {
	return GameplayEffectDenyCount;
}

TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& UAuraAbilitySystemComponent::GetGameplayEffectHandle() {
	return GameplayEffectHandle;
}
