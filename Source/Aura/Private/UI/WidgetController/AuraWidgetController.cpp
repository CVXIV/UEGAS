// Copyright Cvxiv


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WcParams) {
	PlayerController = WcParams.PlayerController;
	PlayerState = WcParams.PlayerState;
	AbilitySystemComponent = WcParams.AbilitySystemComponent;
	AttributeSet = WcParams.AttributeSet;
	InitWidgetController();
}

void UAuraWidgetController::BroadcastInitialValues() {
}

void UAuraWidgetController::BindCallbacksToDependencies() {
}

void UAuraWidgetController::InitWidgetController() {
}
