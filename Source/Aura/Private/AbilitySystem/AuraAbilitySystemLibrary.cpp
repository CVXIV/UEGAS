// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContext) {
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContext, 0)) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD())) {
			AAuraPlayerState* PS = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();
			const FWidgetControllerParams Params(PlayerController, PS, Asc, As);
			return AuraHUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContext) {
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContext, 0)) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD())) {
			AAuraPlayerState* PS = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();
			const FWidgetControllerParams Params(PlayerController, PS, Asc, As);
			return AuraHUD->GetAttributeMenuWidgetController(Params);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* AbilitySystemComponent) {
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (!AuraGameModeBase) {
		return;
	}

	UCharacterClassInfo* CharacterClassInfo = AuraGameModeBase->CharacterClassInfo;
	check(CharacterClassInfo)
	const FCharacterClassDefaultInfo& CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	const AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(AvatarActor);

	const FGameplayEffectSpecHandle& PrimarySpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttributes, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data);

	const FGameplayEffectSpecHandle& SecondarySpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data);

	const FGameplayEffectSpecHandle& VitalSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContext, UAbilitySystemComponent* AbilitySystemComponent, ECharacterClass CharacterClass) {
	// 客户端无法获取
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (!AuraGameModeBase) {
		return;
	}

	UCharacterClassInfo* CharacterClassInfo = AuraGameModeBase->CharacterClassInfo;
	check(CharacterClassInfo)
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterClassInfo->CommonAbilities) {
		if (IsValid(AbilityClass)) {
			FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
			AbilitySystemComponent->GiveAbility(GameplayAbilitySpec);
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AbilitySystemComponent->GetAvatarActor())) {
		if (FCharacterClassDefaultInfo* DefaultInfo = CharacterClassInfo->CharacterClassInformation.Find(CharacterClass)) {
			for (const TSubclassOf<UGameplayAbility>& StartupAbility : DefaultInfo->StartupAbilities) {
				if (IsValid(StartupAbility)) {
					FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(StartupAbility, CombatInterface->GetPlayerLevel());
					AbilitySystemComponent->GiveAbility(GameplayAbilitySpec);
				}
			}
		}
	}
}

float UAuraAbilitySystemLibrary::Sigmoid_Modify(const float X) {
	return (1.0f / (1.0f + exp(-X * 0.03f)) - 0.5f) * 2.f;
}

bool UAuraAbilitySystemLibrary::ProbabilityCheck(const float Probability) {
	return FMath::RandRange(0.f, 1.f) <= Probability;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->IsIsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->IsIsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetIsCriticalHit(bInIsCriticalHit);
	}
}

TArray<AActor*> UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContext, const TArray<AActor*> ActorsToIgnore, float Radius, const FVector& SphereOrigin) {
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TSet<AActor*> OverlappingActors;
	// query scene to see what we hit
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull)) {
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

		for (FOverlapResult& Overlap : Overlaps) {
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor())) {
				OverlappingActors.Add(Overlap.GetActor());
			}
		}
	}

	return OverlappingActors.Array();
}

bool UAuraAbilitySystemLibrary::TryActivateRandomAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation) {
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	return AuraAbilitySystemComponent->TryActivateRandomAbilityByTag(GameplayTagContainer, bAllowRemoteActivation);
}

float UAuraAbilitySystemLibrary::GetXPForCharacterClassAndLevel(const UObject* WorldContext, ECharacterClass CharacterClass, int32 Level) {
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (!AuraGameModeBase) {
		return 0;
	}

	UCharacterClassInfo* CharacterClassInfo = AuraGameModeBase->CharacterClassInfo;
	check(CharacterClassInfo)
	const FCharacterClassDefaultInfo& CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	return CharacterClassDefaultInfo.XPReward.GetValueAtLevel(Level);
}

FGameplayTag UAuraAbilitySystemLibrary::GetAuraAbilityTagFromAbility(const UGameplayAbility* GameplayAbility) {
	for (const FGameplayTag& GameplayTag : GameplayAbility->AbilityTags) {
		if (GameplayTag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability"))) {
			return GameplayTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec) {
	for (const FGameplayTag& GameplayTag : AbilitySpec.DynamicAbilityTags) {
		if (GameplayTag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag"))) {
			return GameplayTag;
		}
	}
	return FGameplayTag();
}
