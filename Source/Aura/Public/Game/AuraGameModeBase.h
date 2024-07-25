// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class USaveGame;
class UMVVM_LoadSlot;
class UCharacterClassInfo;

struct FSlotInfo
{
	FSlotInfo()
	{
	}

	FSlotInfo(const class UEMSInfoSaveGame* InInfoSaveGame, const class UCustomSaveGame* InCustomSaveGame)
	{
		InfoSaveGame = TObjectPtr(*InInfoSaveGame);
		CustomSaveGame = TObjectPtr(*InCustomSaveGame);
	}

	TObjectPtr<const UEMSInfoSaveGame> InfoSaveGame;
	TObjectPtr<const UCustomSaveGame> CustomSaveGame;
};

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<class UAuraDataAssetAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Loot Tiers")
	TObjectPtr<class ULootTiers> LootTiers;

	FSlotInfo SaveSlotData(const FString& SlotIndexName, const FString& SlotName) const;

	bool TryDeleteSlotData(const FString& SlotIndexName) const;

	FSlotInfo GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
};
