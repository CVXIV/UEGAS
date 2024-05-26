// Copyright Cvxiv


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"
#include "AbilitySystem/Data/AuraDataAssetAbilityInfo.h"
#include "Player/AuraPlayerState.h"


void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WcParams) {
	PlayerController = WcParams.PlayerController;
	PlayerState = WcParams.PlayerState;
	AbilitySystemComponent = WcParams.AbilitySystemComponent;
	PlayerAbilitySystemComponent = Cast<UAuraPlayerAbilitySystemComponent>(AbilitySystemComponent);
	AuraAttributeSet = CastChecked<UAuraAttributeSet>(WcParams.AttributeSet);
	InitWidgetController();
}

void UAuraWidgetController::BroadcastInitialValues() {
	const TArray<FGameplayAbilitySpec>& GameplayAbilitySpecs = AbilitySystemComponent->GetActivatableAbilities();
	for (const FGameplayAbilitySpec& AbilitySpec : GameplayAbilitySpecs) {
		OnAbilityChange(AbilitySpec);
	}
}

void UAuraWidgetController::BindCallbacksToDependencies() {
	PlayerAbilitySystemComponent->AbilityChangeDelegate.AddUObject(this, &UAuraWidgetController::OnAbilityChange);
}

void UAuraWidgetController::OnAbilityChange(const FGameplayAbilitySpec& AbilitySpec) {
	if (IsValid(AbilitySpec.Ability)) {
		const FGameplayTag& AbilityTag = UAuraAbilitySystemLibrary::GetAbilityTagFromAbility(AbilitySpec.Ability);
		if (UAuraAbilitySystemLibrary::IsTagValid(AbilityTag)) {
			FAuraDataAssetAbilityInfoRow AbilityInfoRow = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			AbilityInfoRow.InputTag = UAuraAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			AbilityInfoRow.AbilityStatus = UAuraAbilitySystemLibrary::GetAbilityStatusFromSpec(AbilitySpec);
			AbilityChangeDelegate.Broadcast(AbilityInfoRow);
		}
	}
}

void UAuraWidgetController::InitWidgetController() {
	AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
}
