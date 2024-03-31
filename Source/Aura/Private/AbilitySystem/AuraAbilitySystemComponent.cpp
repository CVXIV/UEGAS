// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Character/AuraCharacterBase.h"

TMap<TSubclassOf<UGameplayEffect>, int32>& UAuraAbilitySystemComponent::GetGameplayEffectDenyCount() {
	return GameplayEffectDenyCount;
}

TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& UAuraAbilitySystemComponent::GetGameplayEffectHandle() {
	return GameplayEffectHandle;
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<FAbilityInfo>& StartupAbilities) {
	for (const FAbilityInfo& AbilityInfo : StartupAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, AbilityInfo.Level);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability)) {
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
			//GiveAbilityAndActivateOnce(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityActorInfoSet() {
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag) {
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) {
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) {
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag) {
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) {
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) {
	FGameplayTagContainer GameplayTagContainer;
	GameplayEffectSpec.GetAllAssetTags(GameplayTagContainer);

	EffectAssetTags.Broadcast(GameplayTagContainer);
}
