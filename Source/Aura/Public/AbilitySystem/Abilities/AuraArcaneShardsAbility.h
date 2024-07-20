// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AuraArcaneShardsAbility.generated.h"

class APointCollection;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinished);

UCLASS()
class UAbilityTask_SpawnArcaneShards : public UAbilityTask {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnFinished OnSuccess;

	UFUNCTION(BlueprintCallable, meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category="Ability|Tasks")
	static UAbilityTask_SpawnArcaneShards* SpawnArcaneShardsAndTakeDamage(UGameplayAbility* OwningAbility, const TArray<FVector>& Locations);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** Callback function for when the owning Gameplay Ability is cancelled */
	UFUNCTION()
	void OnPreDestroy();

	void SpawnArcaneShards_Internal(const FVector& Location) const;

private:
	UPROPERTY()
	TObjectPtr<UAuraDamageGameplayAbility> ArcaneShardsAbility;

	TArray<FVector> SpawnLocations;

	FDelegateHandle InterruptedHandle;

	FTimerHandle TimerHandle;

	int32 CurrentLoop;
};

UCLASS()
class AURA_API UAuraArcaneShardsAbility : public UAuraDamageGameplayAbility {
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FVector> GetRandomSpawnLocations(const FVector& Location);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APointCollection> PointCollectionClass;

	UPROPERTY()
	TObjectPtr<APointCollection> PointCollection;
};
