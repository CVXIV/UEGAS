// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

class AAuraEffectActor;

USTRUCT(BlueprintType)
struct FLootItem {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TSubclassOf<AAuraEffectActor> LootClass;

	UPROPERTY(EditAnywhere, Category = "LootTiers|Spawning", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float ChanceToSpawn = 0.0f;

	UPROPERTY(EditAnywhere, Category = "LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

UCLASS()
class AURA_API ULootTiers : public UDataAsset {
	GENERATED_BODY()

public:
	TArray<FLootItem> GetLootItems() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LootTiers|Spawning")
	TArray<FLootItem> LootItems;
};
