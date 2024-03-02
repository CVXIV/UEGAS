// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()

public:
	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, int32>& GetGameplayEffectDenyCount();

	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& GetGameplayEffectHandle();

private:
	TMap<TSubclassOf<UGameplayEffect>, int32> GameplayEffectDenyCount;

	TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>> GameplayEffectHandle;
};
