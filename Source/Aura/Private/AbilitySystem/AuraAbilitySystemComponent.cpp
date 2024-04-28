// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

TMap<TSubclassOf<UGameplayEffect>, int32>& UAuraAbilitySystemComponent::GetGameplayEffectDenyCount() {
	return GameplayEffectDenyCount;
}

TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& UAuraAbilitySystemComponent::GetGameplayEffectHandle() {
	return GameplayEffectHandle;
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

bool UAuraAbilitySystemComponent::TryActivateRandomAbilityByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation) {
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate);

	bool bSuccess = false;

	while (!bSuccess && !AbilitiesToActivate.IsEmpty()) {
		const int Index = FMath::RandRange(0, AbilitiesToActivate.Num() - 1);
		AbilitiesToActivate[Index];
		bSuccess = TryActivateAbility(AbilitiesToActivate[Index]->Handle, bAllowRemoteActivation);
		AbilitiesToActivate.RemoveAt(Index);
	}

	return bSuccess;
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) {
	FGameplayTagContainer GameplayTagContainer;
	GameplayEffectSpec.GetAllAssetTags(GameplayTagContainer);

	EffectAssetTags.Broadcast(GameplayTagContainer);
}
