// Copyright Cvxiv


#include "AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const {
	for (const FAuraAttributeInfo& Info : AttributeInfoInformation) {
		if (Info.AttributeTag.MatchesTag(AttributeTag)) {
			return Info;
		}
	}
	if (bLogNotFound) {
		UE_LOG(LogAura, Error, TEXT("Cannot find Info for AttributeTag [%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
