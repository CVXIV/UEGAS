// Copyright Cvxiv


#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AuraDataAssetAbilityInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"


void UAuraPlayerAbilitySystemComponent::AddCharacterAbilities(const TArray<FAbilityDetail>& StartupAbilities) {
	for (const FAbilityDetail& AbilityInfo : StartupAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, AbilityInfo.Level);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability)) {
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraPlayerAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<FAbilityDetail>& StartupPassiveAbilities) {
	for (const FAbilityDetail& AbilityInfo : StartupPassiveAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, AbilityInfo.Level);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

FGameplayAbilitySpec* UAuraPlayerAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag) {
	TArray<FGameplayAbilitySpec>& AbilitySpecs = GetActivatableAbilities();
	for (FGameplayAbilitySpec& AbilitySpec : AbilitySpecs) {
		if (AbilitySpec.Ability->AbilityTags.HasTag(AbilityTag)) {
			return &AbilitySpec;
		}
	}
	return nullptr;
}

void UAuraPlayerAbilitySystemComponent::UpdateAbilityStatus(int32 Level) {
	const UAuraDataAssetAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityInfo) {
		return;
	}
	FScopedAbilityListLock ActiveScopeLoc(*this);
	const TArray<FGameplayAbilitySpec>& AbilitySpecs = GetActivatableAbilities();
	for (const FAuraDataAssetAbilityInfoRow& Info : AbilityInfo->AbilityInformation) {
		if (Info.AbilityTag.IsValid() && Level >= Info.LevelRequirement) {
			bool bAlreadyHave = false;
			for (const FGameplayAbilitySpec& AbilitySpec : AbilitySpecs) {
				if (AbilitySpec.Ability->GetClass() == Info.Ability) {
					bAlreadyHave = true;
					break;
				}
			}
			if (!bAlreadyHave) {
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Eligible);
				GiveAbility(AbilitySpec);
				// 强制进行Replicate，而不是等到下一次更新
				MarkAbilitySpecDirty(AbilitySpec);
			}
		}
	}
}

void UAuraPlayerAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag) {
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (!AbilitySpec) {
		return;
	}
	for (const FGameplayTag& GameplayTag : AbilitySpec->DynamicAbilityTags) {
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Locked)) {
			break;
		}
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Eligible)) {
			if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetAvatarActor())) {
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTag);
				AbilitySpec->DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Unlocked);
				PlayerInterface->AddToSpellPoints(-1);

				ClientUpdateAbilityStatus(AbilityTag, FAuraGameplayTags::Get().Ability_Status_Unlocked);
				MarkAbilitySpecDirty(*AbilitySpec);
			}
			break;
		}
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Unlocked) || GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Equipped)) {
			if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetAvatarActor())) {
				AbilitySpec->Level += 1;
				PlayerInterface->AddToSpellPoints(-1);

				MarkAbilitySpecDirty(*AbilitySpec);
			}
			break;
		}
	}
}

void UAuraPlayerAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityStatusTag) {
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag)) {
		if (GetAvatarActor()->HasAuthority()) {
			AbilityChangeDelegate.Broadcast(*AbilitySpec);
		} else {
			FGameplayAbilitySpec TempAbilitySpec = *AbilitySpec;
			TempAbilitySpec.DynamicAbilityTags.Reset();
			TempAbilitySpec.DynamicAbilityTags.AddTag(AbilityStatusTag);
			AbilityChangeDelegate.Broadcast(TempAbilitySpec);
		}
	}
}

void UAuraPlayerAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& AbilitySlot) {
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag)) {
		const EAbilityStatus AbilityStatus = UAuraAbilitySystemLibrary::GetAbilityStatusFromSpec(*AbilitySpec);
		if (AbilityStatus == EAbilityStatus::Equipped || AbilityStatus == EAbilityStatus::Unlocked) {
			const FGameplayTag& PreSlot = UAuraAbilitySystemLibrary::GetInputTagFromSpec(*AbilitySpec);
			if (!PreSlot.MatchesTagExact(AbilitySlot)) {
				// 清除原先的输入绑定
				AbilitySpec->DynamicAbilityTags.RemoveTag(PreSlot);
				// 清除将要绑定的输入（如果有的话）
				ClearAbilitiesOfSlot(AbilitySlot);
				AbilitySpec->DynamicAbilityTags.AddTag(AbilitySlot);
				if (AbilityStatus == EAbilityStatus::Unlocked) {
					AbilitySpec->DynamicAbilityTags.RemoveTag(FAuraGameplayTags::Get().Ability_Status_Unlocked);
					AbilitySpec->DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
				}
				MarkAbilitySpecDirty(*AbilitySpec);
				ClientEquipAbility(AbilityTag, AbilitySlot, PreSlot);
			}
		}
	}
}

void UAuraPlayerAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot, const FGameplayTag& PreSlot) {
	AbilityEquippedDelegate.Broadcast(AbilityTag, Slot, PreSlot);
}

bool UAuraPlayerAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription) {
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag)) {
		if (const UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(AbilitySpec->Ability)) {
			OutDescription = Ability->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = Ability->GetDescription(AbilitySpec->Level + 1);
			return true;
		}
	}

	if (UAuraAbilitySystemLibrary::IsTagValid(AbilityTag)) {
		if (const UAuraDataAssetAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())) {
			OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
		}
	} else {
		OutDescription = FString();
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraPlayerAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) {
	Super::OnGiveAbility(AbilitySpec);
	AbilityChangeDelegate.Broadcast(AbilitySpec);
}

void UAuraPlayerAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot) {
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities()) {
		Spec.DynamicAbilityTags.RemoveTag(Slot);
	}
}
