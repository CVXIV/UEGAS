// Copyright Cvxiv


#include "AbilitySystem/Data/AuraDataAssetLevelUpInfo.h"

uint32 UAuraDataAssetLevelUpInfo::FindLevelForXP(uint32 XP) {
	uint32 Level = 0;
	for (const FDataAssetLevelUpInfoRow& Row : LevelUpInfo) {
		if (XP < Row.LevelUpRequirement) {
			break;
		}
		Level++;
	}
	return Level;
}
