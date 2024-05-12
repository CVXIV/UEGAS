// Copyright Cvxiv


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraDataAssetAbilityInfo.h"
#include "AbilitySystem/Data/AuraDataAssetLevelUpInfo.h"
#include "Player/AuraPlayerState.h"


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

	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);

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
		if (AbilityTag.IsValid()) {
			FAuraDataAssetAbilityInfoRow AbilityInfoRow = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			AbilityInfoRow.InputTag = UAuraAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			AbilityGivenDelegate.Broadcast(AbilityInfoRow);
		}
	}
}

void UOverlayWidgetController::InitWidgetController() {
	Super::InitWidgetController();
	AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
}

void UOverlayWidgetController::OnXPChanged(uint32 NewXP, uint32 OldXP) const {
	const uint32 Level = AuraPlayerState->DataAssetLevelUpInfo->FindLevelForXp(OldXP);
	const uint32 NewLevel = AuraPlayerState->DataAssetLevelUpInfo->FindLevelForXp(NewXP);
	const uint32 QueryLevel = NewLevel != Level ? NewLevel : Level;
	const uint32 RequirementXP = AuraPlayerState->DataAssetLevelUpInfo->LevelUpInfo[QueryLevel].LevelUpRequirement;
	const uint32 PreRequirementXP = AuraPlayerState->DataAssetLevelUpInfo->LevelUpInfo[QueryLevel - 1].LevelUpRequirement;
	const float XPPercentage = FMath::Clamp(static_cast<float>(NewXP - PreRequirementXP) / (RequirementXP - PreRequirementXP), 0.f, 1.f);
	OnXPPercentageChangedDelegate.Broadcast(XPPercentage);
}

void UOverlayWidgetController::OnLevelChanged(uint32 NewLevel, uint32 OldLevel) const {
	OnLevelChangedDelegate.Broadcast(NewLevel);
}
