// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;

UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility {
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();

	UPROPERTY(EditDefaultsOnly)
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
