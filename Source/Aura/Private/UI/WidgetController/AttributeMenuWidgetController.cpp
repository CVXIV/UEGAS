// Copyright Cvxiv


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues() {
	Super::BroadcastInitialValues();

	check(AttributeInfo)

	for (auto& Item : AuraAttributeSet->TagsToAttribute) {
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Item.Key);
		Info.AttributeValue = Item.Value().GetNumericValue(AuraAttributeSet);
		OnAttributeChanged.Broadcast(Info);
	}

	AttributePointsChangedDelegate.Broadcast(CastChecked<AAuraPlayerState>(PlayerState)->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies() {
	Super::BindCallbacksToDependencies();

	for (auto& Item : AuraAttributeSet->TagsToAttribute) {
		// 按值捕获Item，因为当事件发生的时候，Item引用已经过期了，因此需要拷贝一个
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Item.Value()).AddLambda([this, Item](const FOnAttributeChangeData& Data) {
			FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Item.Key);
			Info.AttributeValue = Item.Value().GetNumericValue(AuraAttributeSet);
			OnAttributeChanged.Broadcast(Info);
		});
	}

	CastChecked<AAuraPlayerState>(PlayerState)->OnAttributePointsChangedDelegate.AddLambda([this](uint32 NewValue, uint32 OldValue) {
		AttributePointsChangedDelegate.Broadcast(NewValue);
	});
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag) {
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::InitWidgetController() {
	Super::InitWidgetController();
}
