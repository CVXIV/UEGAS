// Copyright Cvxiv


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const {
	for (const auto& [Action, Tag] : AbilityInputAction) {
		if (Tag.MatchesTagExact(InputTag)) {
			return Action;
		}
	}
	if (bLogNotFound) {
		UE_LOG(LogTemp, Error, TEXT("Cannot find Info for InputAction [%s] on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
