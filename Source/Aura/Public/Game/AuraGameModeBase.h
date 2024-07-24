// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCustomSaveGame;

class USaveGame;
class UMVVM_LoadSlot;
class UCharacterClassInfo;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<class UAuraDataAssetAbilityInfo> AbilityInfo;

	void SaveSlotData(const FString& SlotName) const;

	bool TryDeleteSlotData(const FString& SlotName) const;

	UCustomSaveGame* GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
};
