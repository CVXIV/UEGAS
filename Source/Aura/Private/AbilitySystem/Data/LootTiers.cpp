// Copyright Cvxiv


#include "AbilitySystem/Data/LootTiers.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

TArray<FLootItem> ULootTiers::GetLootItems() const {
	TArray<FLootItem> ReturnItems;
	for (const FLootItem& LootItem : LootItems) {
		for (int i = 0; i < LootItem.MaxNumberToSpawn; ++i) {
			if (UAuraAbilitySystemLibrary::ProbabilityCheck(LootItem.ChanceToSpawn)) {
				FLootItem NewItem;
				NewItem.LootClass = LootItem.LootClass;
				NewItem.bLootLevelOverride = LootItem.bLootLevelOverride;
				ReturnItems.Add(NewItem);
			}
		}
	}
	return ReturnItems;
}
