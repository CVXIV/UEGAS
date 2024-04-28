// Copyright Cvxiv


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraDataAssetAbilityInfo.h"


void UOverlayWidgetController::BroadcastInitialValues() {
	Super::BroadcastInitialValues();

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies() {
	Super::BindCallbacksToDependencies();
	GAMEPLAY_ATTRIBUTE_ON_CHANGE(Health)
	GAMEPLAY_ATTRIBUTE_ON_CHANGE(MaxHealth)
	GAMEPLAY_ATTRIBUTE_ON_CHANGE(Mana)
	GAMEPLAY_ATTRIBUTE_ON_CHANGE(MaxMana)

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)) {
		AuraAbilitySystemComponent->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags) {
			for (const FGameplayTag& GameplayTag : AssetTags) {
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag("Message");
				if (GameplayTag.MatchesTag(MessageTag)) {
					if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, GameplayTag)) {
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		});
	}
}

void UOverlayWidgetController::OnAbilityGiven(const FGameplayAbilitySpec& AbilitySpec) const {
	if (IsValid(AbilitySpec.Ability)) {
		const FGameplayTag AbilityTag = UAuraAbilitySystemLibrary::GetAuraAbilityTagFromAbility(AbilitySpec.Ability);
			FAuraDataAssetAbilityInfoRow AbilityInfoRow = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			AbilityInfoRow.InputTag = UAuraAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			AbilityGivenDelegate.Broadcast(AbilityInfoRow);
	}
}

void UOverlayWidgetController::InitWidgetController() {
	Super::InitWidgetController();
}
