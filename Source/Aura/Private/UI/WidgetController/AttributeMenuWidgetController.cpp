// Copyright Cvxiv


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeMenuWidgetController::BroadcastInitialValues() {
	Super::BroadcastInitialValues();

	check(AttributeInfo)

	for (auto& Item : AuraAttributeSet->TagsToAttribute) {
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Item.Key);
		Info.AttributeValue = Item.Value().GetNumericValue(AuraAttributeSet);
		OnAttributeChanged.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies() {
	Super::BindCallbacksToDependencies();

	for (auto& Item : AuraAttributeSet->TagsToAttribute) {
		// 按值捕获Item，因为当事件发生的时候，Item引用已经过期了，因此需要拷贝一个
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Item.Value()).AddLambda([this,Item](const FOnAttributeChangeData& Data) {
			FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Item.Key);
			Info.AttributeValue = Item.Value().GetNumericValue(AuraAttributeSet);
			OnAttributeChanged.Broadcast(Info);
		});
	}
}

void UAttributeMenuWidgetController::InitWidgetController() {
	Super::InitWidgetController();
}
