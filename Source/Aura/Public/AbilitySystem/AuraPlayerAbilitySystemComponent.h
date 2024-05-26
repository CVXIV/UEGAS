// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraPlayerAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityChange, const FGameplayAbilitySpec&);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityEquipped, const FGameplayTag& AbilityTag, const FGameplayTag& Slot, const FGameplayTag& PreSlot);

UCLASS()
class AURA_API UAuraPlayerAbilitySystemComponent : public UAuraAbilitySystemComponent {
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<struct FAbilityDetail>& StartupAbilities);

	void AddCharacterPassiveAbilities(const TArray<struct FAbilityDetail>& StartupPassiveAbilities);

	FAbilityChange AbilityChangeDelegate;

	FAbilityEquipped AbilityEquippedDelegate;

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	void UpdateAbilityStatus(int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityStatusTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& AbilitySlot);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot, const FGameplayTag& PreSlot);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

private:
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
};
