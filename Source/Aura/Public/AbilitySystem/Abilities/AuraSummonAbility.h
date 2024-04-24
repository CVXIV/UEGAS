// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

class UNiagaraSystem;

class AAuraCharacterBase;

UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility {
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	float SpawnSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	float MinSpawnDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	float MaxSpawnDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	uint32 NumMinions = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	TArray<TSubclassOf<AAuraCharacterBase>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Property")
	TObjectPtr<UNiagaraSystem> SpawnTipNiagara;

	UFUNCTION(BlueprintCallable)
	void SpawnMinion();

private:
	TArray<FVector> GetSpawnLocations() const;

	void SpawnMinion_Internal(const FVector& SpawnLocation);
};
