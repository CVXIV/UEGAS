// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UDeBuffNiagaraComponent;

class UGameplayAbility;

class UGameplayEffect;

class UAuraAbilitySystemComponent;

class UAttributeSet;

USTRUCT()
struct FAbilityDetail {
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Combat Interface*/
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;

	virtual bool IsDead_Implementation() const override;

	virtual AActor* GetAvatar_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Die(const FVector& DeathImpulse) override;

	virtual ECharacterClass GetCharacterClass_Implementation() const override;

	virtual FTaggedMontage GetAttackMontage_Random_Implementation() const override;

	virtual FTaggedMontage GetAttackMontageByTag_Implementation(const FGameplayTag& Tag) const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;

	virtual FASCRegisterSignature& GetOnAscRegisteredDelegate() override;

	virtual FDiedSignature& GetOnDiedDelegate() override;

	virtual void SetInShockLoop_Implementation(bool InbInShockLoop) override;

	virtual USkeletalMeshComponent* GetWeapon() const override;

	/** Combat Interface*/

	UAttributeSet* GetAttributeSet() const {
		return AttributeSet;
	}

protected:
	virtual void BeginPlay() override;

	virtual void OnDie();

	virtual void InitAbilityActorInfo();

	virtual void PostAbilitySystemInit();

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void OnPassiveAbilityChange(const FGameplayTag& AbilityTag, bool bActivate);

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffect, float Level) const;

	virtual void InitializeDefaultAttributes() const;

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

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseWalkSpeed = 600.f;

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

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bInShockLoop = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bStunned = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bBurned = false;

	bool bDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true"))
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	FASCRegisterSignature OnAscRegistered;

	FDiedSignature OnDied;
};
