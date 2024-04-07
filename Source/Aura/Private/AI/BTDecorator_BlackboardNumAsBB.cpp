// Copyright Cvxiv


#include "AI/BTDecorator_BlackboardNumAsBB.h"

#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_BlackboardNumAsBB::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	// note that this may produce unexpected logical results. FALSE is a valid return value here as well
	// @todo signal it
	return BlackboardComp && EvaluateOnBlackboard_Custom(*BlackboardComp);
}

bool UBTDecorator_BlackboardNumAsBB::EvaluateOnBlackboard_Custom(const UBlackboardComponent& BlackboardComp) const {
	bool bResult = false;
	if (BlackboardKey.SelectedKeyType) {
		UBlackboardKeyType* KeyCDO = BlackboardKey.SelectedKeyType->GetDefaultObject<UBlackboardKeyType>();
		const uint8* KeyMemory = BlackboardComp.GetKeyRawData(BlackboardKey.GetSelectedKeyID());

		// KeyMemory can be NULL if the blackboard has its data setup wrong, so we must conditionally handle that case.
		if (ensure(KeyCDO != NULL) && (KeyMemory != NULL)) {
			const EBlackboardKeyOperation::Type Op = KeyCDO->GetTestOperation();
			switch (Op) {
			case EBlackboardKeyOperation::Basic:
				bResult = KeyCDO->WrappedTestBasicOperation(BlackboardComp, KeyMemory, (EBasicKeyOperation::Type)OperationType);
				break;

			case EBlackboardKeyOperation::Arithmetic: {
				const float RealFloatValue = BlackboardComp.GetValueAsFloat(FloatValueSelector.SelectedKeyName);
				bResult = KeyCDO->WrappedTestArithmeticOperation(BlackboardComp, KeyMemory, (EArithmeticKeyOperation::Type)OperationType, RealFloatValue, RealFloatValue);
				break;
			}

			case EBlackboardKeyOperation::Text:
				bResult = KeyCDO->WrappedTestTextOperation(BlackboardComp, KeyMemory, (ETextKeyOperation::Type)OperationType, StringValue);
				break;

			default:
				break;
			}
		}
	}

	return bResult;
}

#if WITH_EDITOR
void UBTDecorator_BlackboardNumAsBB::BuildDescription() {
	FString BlackboardDesc = "TODO...";
	CachedDescription = BlackboardDesc;
}
#endif
