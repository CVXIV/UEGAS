// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_Attack.generated.h"

class UAbilitySystemComponent;

UCLASS()
class AURA_API UBTTask_Attack : public UBTTask_BlueprintBase {
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
