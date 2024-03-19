// Copyright Cvxiv


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const {
	for (const auto& [InputAction, InputTag] : AbilityInputAction) {
		if (InputTag.MatchesTagExact(InputTag)) {
			return InputAction;
		}
	}
	if (bLogNotFound) {
		UE_LOG(LogTemp, Error, TEXT("Cannot find Info for InputAction [%s] on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
