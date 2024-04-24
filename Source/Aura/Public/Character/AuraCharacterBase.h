// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;

class UGameplayEffect;

class UAuraAbilitySystemComponent;

class UAttributeSet;

USTRUCT()
struct FAbilityInfo {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 Level = 1;
};

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface {
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Combat Interface*/
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;

	virtual bool IsDead_Implementation() const override;

	virtual AActor* GetAvatar_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Die() override;

	virtual FTaggedMontage GetAttackMontage_Random_Implementation() const override;

	virtual FTaggedMontage GetAttackMontageByTag_Implementation(const FGameplayTag& Tag) const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;

	/** Combat Interface*/

	UAttributeSet* GetAttributeSet() const {
		return AttributeSet;
	}

protected:
	virtual void BeginPlay() override;

	virtual void OnDie();

	virtual void InitAbilityActorInfo();

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float Level) const;

	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FTaggedMontage> AttackMontage;

	bool bDead = false;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<FAbilityInfo> StartupAbilities;
};
