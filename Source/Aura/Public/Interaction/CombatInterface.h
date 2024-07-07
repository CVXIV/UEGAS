// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class AAuraCharacterBase;

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FASCRegisterSignature, UAbilitySystemComponent*)

DECLARE_MULTICAST_DELEGATE_OneParam(FDiedSignature, const AAuraCharacterBase*)

enum class ECharacterClass : uint8;

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FTaggedMontage {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface {
	GENERATED_BODY()
};

class AURA_API ICombatInterface {
	GENERATED_BODY()

public:
	virtual int32 GetPlayerLevel() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die(const FVector& DeathImpulse) = 0;

	virtual FASCRegisterSignature& GetOnAscRegisteredDelegate() = 0;

	virtual FDiedSignature& GetOnDiedDelegate() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetAttackMontage_Random() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetAttackMontageByTag(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInShockLoop(bool InbInShockLoop);

	virtual USkeletalMeshComponent* GetWeapon() const = 0;
};
