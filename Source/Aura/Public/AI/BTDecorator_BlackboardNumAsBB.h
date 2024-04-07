// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_BlackboardNumAsBB.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTDecorator_BlackboardNumAsBB : public UBTDecorator_Blackboard {
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	/** value for arithmetic operations */
	UPROPERTY(Category=Blackboard, EditAnywhere, meta=(DisplayName="Key Value"))
	FBlackboardKeySelector FloatValueSelector;

private:
	/** take blackboard value and evaluate decorator's condition */
	bool EvaluateOnBlackboard_Custom(const UBlackboardComponent& BlackboardComp) const;
#if WITH_EDITOR

public:
	/** describe decorator and cache it */
	virtual void BuildDescription() override;
#endif
};
