// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface {
	GENERATED_BODY()

public:
	virtual void HighlightActor() = 0;

	virtual void UnHighlightActor() = 0;

	virtual void SetHealthReplicated() = 0;

	virtual void SetMaxHealthReplicated() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget();
};
