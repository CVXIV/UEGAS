// Copyright Cvxiv


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues() {
	Super::BroadcastInitialValues();
	if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(AbilitySystemComponent->GetAvatarActor())) {
		Dd_SpellPointsChanged.Broadcast(PlayerInterface->GetSpellPoints());
	}
}

void USpellMenuWidgetController::BindCallbacksToDependencies() {
	Super::BindCallbacksToDependencies();
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([this](int32 NewSpellPoints, int32 OldSpellPoints) {
		Dd_SpellPointsChanged.Broadcast(NewSpellPoints);

		SpellGlobeSelected(SelectedAbilityTag);
	});

	PlayerAbilitySystemComponent->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag) {
	SelectedAbilityTag = AbilityTag;
	if (bWaitForEquipSelection) {
		Dd_EndEquip.Broadcast();
		bWaitForEquipSelection = false;
	}

	const int32 SpellPoints = AuraPlayerState->GetSpellPoints();
	const FGameplayAbilitySpec* AbilitySpec = PlayerAbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag);
	bool bSpendPointsButtonEnabled = false;
	bool bEquipButtonEnabled = false;
	if (AbilitySpec) {
		switch (UAuraAbilitySystemLibrary::GetAbilityStatusFromSpec(*AbilitySpec)) {
		case EAbilityStatus::Eligible:
			if (SpellPoints > 0) {
				bSpendPointsButtonEnabled = true;
			}
			break;
		case EAbilityStatus::Equipped:
		case EAbilityStatus::Unlocked:
			if (SpellPoints > 0) {
				bSpendPointsButtonEnabled = true;
			}
			bEquipButtonEnabled = true;
			break;
		case EAbilityStatus::Locked:
		case EAbilityStatus::None:
			break;
		}
	}

	FString Description, NextLevelDescription;
	PlayerAbilitySystemComponent->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	Dd_SpellGlobeSelected.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpellEquipSelected() {
	if (UAuraAbilitySystemLibrary::IsTagValid(SelectedAbilityTag)) {
		const FAuraDataAssetAbilityInfoRow AbilityInfoRow = UAuraAbilitySystemLibrary::GetAbilityInfo(AbilitySystemComponent->GetAvatarActor())->FindAbilityInfoForTag(SelectedAbilityTag);
		EAbilityType AbilityType = EAbilityType::None;
		if (AbilityInfoRow.TypeTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Offensive)) {
			AbilityType = EAbilityType::Offensive;
		} else if (AbilityInfoRow.TypeTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Passive)) {
			AbilityType = EAbilityType::Passive;
		}
		if (AbilityType == EAbilityType::Offensive || AbilityType == EAbilityType::Passive) {
			Dd_WaitForEquip.Broadcast(AbilityType);
			bWaitForEquipSelection = true;

			const FGameplayAbilitySpec* AbilitySpec = PlayerAbilitySystemComponent->GetSpecFromAbilityTag(SelectedAbilityTag);
			const EAbilityStatus AbilityStatus = UAuraAbilitySystemLibrary::GetAbilityStatusFromSpec(*AbilitySpec);
			if (AbilityStatus == EAbilityStatus::Equipped) {
				SelectedSlot = UAuraAbilitySystemLibrary::GetInputTagFromSpec(*AbilitySpec);
			}
		}
	}
}

void USpellMenuWidgetController::OnAbilityChange(const FGameplayAbilitySpec& AbilitySpec) {
	Super::OnAbilityChange(AbilitySpec);
	SpellGlobeSelected(SelectedAbilityTag);
}

void USpellMenuWidgetController::SpendPointsButtonClicked() {
	PlayerAbilitySystemComponent->ServerSpendSpellPoints(SelectedAbilityTag);
}

void USpellMenuWidgetController::OnSpellMenuClosed() {
	SelectedAbilityTag = FGameplayTag();
	SelectedSlot = FGameplayTag();
}

void USpellMenuWidgetController::SpellRowGlobeClicked(const FGameplayTag& SlotTag) {
	if (!bWaitForEquipSelection) {
		return;
	}

	PlayerAbilitySystemComponent->ServerEquipAbility(SelectedAbilityTag, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Slot, const FGameplayTag& PreSlot) {
	bWaitForEquipSelection = false;

	// 该广播的作用是清空之前的输入信息
	FAuraDataAssetAbilityInfoRow PreAbilityInfoRow;
	PreAbilityInfoRow.AbilityStatus = EAbilityStatus::Unlocked;
	PreAbilityInfoRow.InputTag = PreSlot;
	AbilityChangeDelegate.Broadcast(PreAbilityInfoRow);

	FAuraDataAssetAbilityInfoRow NewAbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(AbilitySystemComponent->GetAvatarActor())->FindAbilityInfoForTag(AbilityTag);
	NewAbilityInfo.AbilityStatus = EAbilityStatus::Equipped;
	NewAbilityInfo.InputTag = Slot;
	AbilityChangeDelegate.Broadcast(NewAbilityInfo);

	Dd_EndEquip.Broadcast();
}
