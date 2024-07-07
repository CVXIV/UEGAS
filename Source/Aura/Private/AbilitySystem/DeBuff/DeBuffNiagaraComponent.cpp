// Copyright Cvxiv


#include "AbilitySystem/DeBuff/DeBuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDeBuffNiagaraComponent::UDeBuffNiagaraComponent() {
	bAutoActivate = false;
}

void UDeBuffNiagaraComponent::BeginPlay() {
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	if (UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())) {
		Asc->RegisterGameplayTagEvent(DeBuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
	} else if (CombatInterface) {
		// 添加弱引用，可以防止组件无法被垃圾回收
		CombatInterface->GetOnAscRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InAsc) {
			InAsc->RegisterGameplayTagEvent(DeBuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
		});
	}
}

void UDeBuffNiagaraComponent::DeBuffTagChanged(FGameplayTag CallbackTag, int32 NewCount) {
	if (NewCount > 0 && IsValid(GetOwner()) && GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner())) {
		Activate();
	} else {
		Deactivate();
	}
}
