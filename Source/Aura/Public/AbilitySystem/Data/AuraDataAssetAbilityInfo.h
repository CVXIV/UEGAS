// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraDataAssetAbilityInfo.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class EAbilityStatus : uint8 {
	None,
	Locked,
	Eligible,
	Unlocked,
	Equipped
};

USTRUCT(BlueprintType)
struct FAuraDataAssetAbilityInfoRow {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag TypeTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	EAbilityStatus AbilityStatus = EAbilityStatus::None;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<const UMaterialInstance> BackgroundMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 LevelRequirement = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;
};

UCLASS()
class AURA_API UAuraDataAssetAbilityInfo : public UDataAsset {
	GENERATED_BODY()

public:
	FAuraDataAssetAbilityInfoRow FindAbilityInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = true) const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AbilityInformation")
	TArray<FAuraDataAssetAbilityInfoRow> AbilityInformation;
};
