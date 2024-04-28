// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UAuraPlayerAbilitySystemComponent;

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase {
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual int32 GetPlayerLevel() override;

	virtual void OnRep_PlayerState() override;

protected:
	void AddCharacterAbilities() const;

	UPROPERTY()
	TObjectPtr<UAuraPlayerAbilitySystemComponent> AuraPlayerAbilitySystemComponent;

private:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FAbilityDetail> StartupAbilities;
};
