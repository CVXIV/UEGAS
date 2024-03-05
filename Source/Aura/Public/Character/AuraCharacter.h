// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase {
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual int32 GetPlayerLevel() override;

	virtual void OnRep_PlayerState() override;

private:
	virtual void InitAbilityActorInfo() override;
};
