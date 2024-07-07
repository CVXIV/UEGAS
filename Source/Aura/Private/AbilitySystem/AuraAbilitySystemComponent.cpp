// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/PlayerInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

TMap<TSubclassOf<UGameplayEffect>, int32>& UAuraAbilitySystemComponent::GetGameplayEffectDenyCount() {
	return GameplayEffectDenyCount;
}

TMap<TSubclassOf<UGameplayEffect>, TSharedPtr<TQueue<FActiveGameplayEffectHandle>>>& UAuraAbilitySystemComponent::GetGameplayEffectHandle() {
	return GameplayEffectHandle;
}

void UAuraAbilitySystemComponent::AbilityActorInfoSet() {
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag) {
	if (!InputTag.IsValid()) {
		return;
	}

	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) {
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) {
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag) {
	if (!InputTag.IsValid()) {
		return;
	}

	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) {
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) {
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag) {
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive()) {
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

bool UAuraAbilitySystemComponent::TryActivateRandomAbilityByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation) {
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate);

	bool bSuccess = false;

	while (!bSuccess && !AbilitiesToActivate.IsEmpty()) {
		const int Index = FMath::RandRange(0, AbilitiesToActivate.Num() - 1);
		AbilitiesToActivate[Index];
		bSuccess = TryActivateAbility(AbilitiesToActivate[Index]->Handle, bAllowRemoteActivation);
		AbilitiesToActivate.RemoveAt(Index);
	}

	return bSuccess;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag) {
	if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetAvatarActor())) {
		if (PlayerInterface->GetAttributePoints() > 0) {
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const {
	const UAuraDataAssetAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag& AbilityTag = UAuraAbilitySystemLibrary::GetAbilityTagFromAbility(Spec.Ability);
	return AbilityInfo->FindAbilityInfoForTag(AbilityTag).TypeTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Passive);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag) {
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	Cast<IPlayerInterface>(GetAvatarActor())->AddToAttributePoints(-1);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) {
	FGameplayTagContainer GameplayTagContainer;
	GameplayEffectSpec.GetAllAssetTags(GameplayTagContainer);

	EffectAssetTags.Broadcast(GameplayTagContainer);
}
