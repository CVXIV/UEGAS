// Copyright Cvxiv


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"


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

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags) {
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

void UOverlayWidgetController::InitWidgetController() {
	Super::InitWidgetController();
}
