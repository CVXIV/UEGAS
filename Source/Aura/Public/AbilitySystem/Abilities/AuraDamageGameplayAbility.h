// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"


UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* Instigator, AActor* Target = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (DisplayName = "伤害类型"))
	TMap<FGameplayTag, FDamageTypeInfo> DamageTypes;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float DeathImpulseMagnitude = 0;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float KnockBackForceMagnitude = 0;
};
