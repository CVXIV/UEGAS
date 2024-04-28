// Copyright Cvxiv
#include "AbilitySystem/Data/AuraDataAssetAbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraDataAssetAbilityInfoRow UAuraDataAssetAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const {
	for (const FAuraDataAssetAbilityInfoRow& Info : AbilityInformation) {
		if (Info.AbilityTag.MatchesTag(AttributeTag)) {
			return Info;
		}
	}
	if (bLogNotFound) {
		UE_LOG(LogAura, Error, TEXT("Cannot find Info for AttributeTag [%s] on AbilityInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this))
	}
	return FAuraDataAssetAbilityInfoRow();
}
