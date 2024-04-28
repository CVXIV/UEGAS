// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraPlayerAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraPlayerAbilitySystemComponent : public UAuraAbilitySystemComponent {
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<struct FAbilityDetail>& StartupAbilities);

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
