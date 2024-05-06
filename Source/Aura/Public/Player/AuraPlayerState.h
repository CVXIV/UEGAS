// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraDataAssetLevelUpInfo;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerStateChanged, uint32/*NewValue*/, uint32/*OldValue*/)

class UAuraPlayerAbilitySystemComponent;

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface {
	GENERATED_BODY()

public:
	AAuraPlayerState();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraDataAssetLevelUpInfo> DataAssetLevelUpInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const {
		return AttributeSet;
	}

	void AddToXP(uint32 InXP);

	FOnPlayerStateChanged OnXPChangedDelegate;

	FOnPlayerStateChanged OnLevelChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraPlayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	uint32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	uint32 XP = 0;

public:
	[[nodiscard]] FORCEINLINE uint32 GetPlayerLevel() const {
		return Level;
	}

	[[nodiscard]] FORCEINLINE uint32 GetXP() const {
		return XP;
	}

private:
	UFUNCTION()
	void OnRep_Level(uint32 OldLevel) const;

	UFUNCTION()
	void OnRep_XP(uint32 OldXP) const;
};
