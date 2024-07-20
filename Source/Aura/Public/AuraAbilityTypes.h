#pragma once

#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageTypeInfo {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float DeBuffChance = 0;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float DeBuffDamage = 0;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float DeBuffFrequency = 0;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float DeBuffDuration = 0;
};

USTRUCT()
struct FDeBuffProperty {
	GENERATED_BODY()
	FDeBuffProperty() {
	}

	FDeBuffProperty(bool bInIsSuccessfulDeBuff, const FGameplayTag& InDeBuff, float InDeBuffDamage, float InDeBuffFrequency, float InDeBuffDuration) {
		this->bIsSuccessfulDeBuff = bInIsSuccessfulDeBuff;
		this->DeBuff = InDeBuff;
		this->DeBuffDamage = InDeBuffDamage;
		this->DeBuffFrequency = InDeBuffFrequency;
		this->DeBuffDuration = InDeBuffDuration;
	}

	UPROPERTY()
	bool bIsSuccessfulDeBuff = false;

	UPROPERTY()
	FGameplayTag DeBuff;

	UPROPERTY()
	float DeBuffDamage = 0;

	UPROPERTY()
	float DeBuffFrequency = 0;

	UPROPERTY()
	float DeBuffDuration = 0;

	friend FArchive& operator<<(FArchive& Ar, FDeBuffProperty& DeBuffProperty) {
		return Ar << DeBuffProperty.bIsSuccessfulDeBuff << DeBuffProperty.DeBuffDamage << DeBuffProperty.DeBuffFrequency << DeBuffProperty.DeBuffDuration;
	}
};

USTRUCT(BlueprintType)
struct FDamageEffectParams {
	GENERATED_BODY()

	FDamageEffectParams() {
	}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	UPROPERTY()
	TArray<FGameplayTag> DamageTypesKeys;

	UPROPERTY()
	TArray<FDamageTypeInfo> DamageTypesValues;

	UPROPERTY()
	float AbilityLevel = 1.f;

	UPROPERTY()
	float DeathImpulseMagnitude = 0;

	UPROPERTY()
	float KnockBackForceMagnitude = 0;

	UPROPERTY()
	bool bTakeHitReact = false;

	UPROPERTY()
	bool bIsRadialDamage = false;

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY()
	float RadiusDamageOuterRadius = 0.f;

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext {

	GENERATED_BODY()

	virtual UScriptStruct* GetScriptStruct() const override {
		return StaticStruct();
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	[[nodiscard]] bool IsIsBlockedHit() const {
		return bIsBlockedHit;
	}

	void SetIsBlockedHit(const bool bInIsBlockedHit) {
		this->bIsBlockedHit = bInIsBlockedHit;
	}

	[[nodiscard]] bool IsIsCriticalHit() const {
		return bIsCriticalHit;
	}

	void SetIsCriticalHit(const bool bInIsCriticalHit) {
		this->bIsCriticalHit = bInIsCriticalHit;
	}

	[[nodiscard]] bool IsTakeHitReact() const {
		return bTakeHitReact;
	}

	void SetTakeHitReact(const bool bInTakeHitReact) {
		this->bTakeHitReact = bInTakeHitReact;
	}

	void AddDeBuffInfo(bool bIsSuccessfulDeBuff, const FGameplayTag& InDeBuff, float DeBuffDamage, float DeBuffFrequency, float DeBuffDuration) {
		this->DeBuffProperties.Add(FDeBuffProperty(bIsSuccessfulDeBuff, InDeBuff, DeBuffDamage, DeBuffFrequency, DeBuffDuration));
	}

	const TArray<FDeBuffProperty>& GetDeBuffProperties() const {
		return DeBuffProperties;
	}

	[[nodiscard]] const FVector& GetDeathImpulse() const {
		return DeathImpulse;
	}

	void SetDeathImpulse(const FVector& InDeathImpulse) {
		this->DeathImpulse = InDeathImpulse;
	}

	[[nodiscard]] const FVector& GetKnockBackForce() const {
		return KnockBackForce;
	}

	void SetKnockBackForce(const FVector& InKnockBackForce) {
		this->KnockBackForce = InKnockBackForce;
	}
	[[nodiscard]] bool IsRadialDamage() const {
		return bIsRadialDamage;
	}

	void SetIsRadialDamage(const bool bInIsRadialDamage) {
		this->bIsRadialDamage = bInIsRadialDamage;
	}

	[[nodiscard]] float GetRadialDamageInnerRadius() const {
		return RadialDamageInnerRadius;
	}

	void SetRadialDamageInnerRadius(const float InRadialDamageInnerRadius) {
		this->RadialDamageInnerRadius = InRadialDamageInnerRadius;
	}

	[[nodiscard]] float GetRadiusDamageOuterRadius() const {
		return RadiusDamageOuterRadius;
	}

	void SetRadiusDamageOuterRadius(const float InRadiusDamageOuterRadius) {
		this->RadiusDamageOuterRadius = InRadiusDamageOuterRadius;
	}

	[[nodiscard]] const FVector& GetRadialDamageOrigin() const {
		return RadialDamageOrigin;
	}

	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) {
		this->RadialDamageOrigin = InRadialDamageOrigin;
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FGameplayEffectContext* Duplicate() const override {
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult()) {
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bTakeHitReact = false;

	UPROPERTY()
	TArray<FDeBuffProperty> DeBuffProperties;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;

	UPROPERTY()
	bool bIsRadialDamage = false;

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY()
	float RadiusDamageOuterRadius = 0.f;

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

template <>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext> {
	enum {
		WithNetSerializer = true,
		WithCopy = true
	};
};
