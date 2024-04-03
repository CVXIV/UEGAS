#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext {
	GENERATED_BODY()

public:
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
};

template <>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext> {
	enum {
		WithNetSerializer = true,
		WithCopy = true
	};
};
