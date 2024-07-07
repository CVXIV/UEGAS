// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FAuraGameplayEffectSpec;

struct FAuraGameplayEffectSpecHandle;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTag, const FGameplayTagContainer&);

DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveAbilitySignature, const FGameplayTag&, bool);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()

public:
	UAuraAbilitySystemComponent();

	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, int32>& GetGameplayEffectDenyCount();

	[[nodiscard]] TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& GetGameplayEffectHandle();

	void AbilityActorInfoSet();

	FEffectAssetTag EffectAssetTags;

	FActivatePassiveAbilitySignature ActivatePassiveAbility;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	bool TryActivateRandomAbilityByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;

private:
	TMap<TSubclassOf<UGameplayEffect>, int32> GameplayEffectDenyCount;

	TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>> GameplayEffectHandle;
};
