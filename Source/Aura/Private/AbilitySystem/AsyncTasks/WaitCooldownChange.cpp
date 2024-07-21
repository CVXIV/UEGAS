// Copyright Cvxiv


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag) {
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid()) {
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask() {
	if (!IsValid(ASC)) {
		return;
	}
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

float UWaitCooldownChange::GetTimeRemaining() {
	if (IsValid(ASC) && ActiveGEHandle.IsValid()) {
		const FActiveGameplayEffect* ActiveGameplayEffect = ASC->GetActiveGameplayEffect(ActiveGEHandle);
		return ActiveGameplayEffect->GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds());
	}
	return 0;
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount) const {
	if (NewCount == 0) {
		CooldownEnd.Broadcast(0);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) {
	FGameplayTagContainer AssetTags;
	GameplayEffectSpec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	GameplayEffectSpec.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag)) {
		this->ActiveGEHandle = ActiveGameplayEffectHandle;
		//TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		CooldownStart.Broadcast(GameplayEffectSpec.GetDuration());
	}
}

void UWaitCooldownChange::Activate() {
	TArray<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles = ASC->GetActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(CooldownTag)));
	if (ActiveGameplayEffectHandles.Num() > 0) {
		this->ActiveGEHandle = ActiveGameplayEffectHandles[0];
		this->CooldownStart.Broadcast(GetTimeRemaining());
	}
}
