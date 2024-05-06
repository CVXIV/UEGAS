// Copyright Cvxiv


#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Character/AuraCharacterBase.h"
#include "UI/WidgetController/OverlayWidgetController.h"


class UAuraGameplayAbility;

void UAuraPlayerAbilitySystemComponent::AddCharacterAbilities(const TArray<FAbilityDetail>& StartupAbilities) {
	for (const FAbilityDetail& AbilityInfo : StartupAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, AbilityInfo.Level);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability)) {
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraPlayerAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<FAbilityDetail>& StartupPassiveAbilities) {
	for (const FAbilityDetail& AbilityInfo : StartupPassiveAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, AbilityInfo.Level);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraPlayerAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) {
	Super::OnGiveAbility(AbilitySpec);
	const UOverlayWidgetController* OverlayWidgetController = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	OverlayWidgetController->OnAbilityGiven(AbilitySpec);
}
