// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraDataAssetLevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FDataAssetLevelUpInfoRow {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	uint32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly)
	uint32 AttributePointReward = 1;

	UPROPERTY(EditDefaultsOnly)
	uint32 SpellPointReward = 1;
};

UCLASS()
class AURA_API UAuraDataAssetLevelUpInfo : public UDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FDataAssetLevelUpInfoRow> LevelUpInfo;

	uint32 FindLevelForXp(uint32 XP);
};
