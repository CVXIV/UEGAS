// Copyright Cvxiv


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility) {
	UTargetDataUnderMouse* Obj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);

	return Obj;
}

void UTargetDataUnderMouse::Activate() {
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled()) {
		SendMouseCursorData();
	} else {
		// 服务器接收来自客户端的数据
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		FAbilityTargetDataSetDelegate& TargetDataSetDelegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey);
		TargetDataSetDelegate.AddLambda([this,SpecHandle,ActivationPredictionKey](const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag GameplayTag) {
			AbilitySystemComponent->ConsumeClientReplicatedTargetData(SpecHandle, ActivationPredictionKey);
			if (ShouldBroadcastAbilityTaskDelegates()) {
				ValidData.Broadcast(DataHandle);
			}
		});
		const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate) {
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData() const {
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Ability->GetCurrentActorInfo()->PlayerController.Get()->GetHitResultUnderCursor(ECC_Visibility, false, Data->HitResult);

	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates()) {
		ValidData.Broadcast(DataHandle);
	}
}
