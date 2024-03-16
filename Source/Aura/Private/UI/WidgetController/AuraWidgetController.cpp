// Copyright Cvxiv


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WcParams) {
	PlayerController = WcParams.PlayerController;
	PlayerState = WcParams.PlayerState;
	AbilitySystemComponent = WcParams.AbilitySystemComponent;
	AuraAttributeSet = CastChecked<UAuraAttributeSet>(WcParams.AttributeSet);
	InitWidgetController();
}

void UAuraWidgetController::BroadcastInitialValues() {
}

void UAuraWidgetController::BindCallbacksToDependencies() {
}

void UAuraWidgetController::InitWidgetController() {
}
