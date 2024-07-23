// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULoadScreenSaveGame;
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

	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot) const;

	static bool TryDeleteSlotData(const UMVVM_LoadSlot* LoadSlot);

	static ULoadScreenSaveGame* GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
};
