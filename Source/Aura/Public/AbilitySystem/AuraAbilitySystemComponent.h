// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTag, const FGameplayTagContainer&);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()

public:
	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, int32>& GetGameplayEffectDenyCount();

	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& GetGameplayEffectHandle();

	void AddCharacterAbilities(const TArray<struct FAbilityInfo>& StartupAbilities);

	void AbilityActorInfoSet();

	FEffectAssetTag EffectAssetTags;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	bool TryActivateRandomAbilityByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation);

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

private:
	TMap<TSubclassOf<UGameplayEffect>, int32> GameplayEffectDenyCount;

	TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>> GameplayEffectHandle;
};
