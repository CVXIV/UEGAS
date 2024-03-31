// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContext, UAbilitySystemComponent* AbilitySystemComponent) {
	// 客户端无法获取
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (!AuraGameModeBase) {
		return;
	}

	UCharacterClassInfo* CharacterClassInfo = AuraGameModeBase->CharacterClassInfo;
	check(CharacterClassInfo)
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterClassInfo->CommonAbilities) {
		FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(GameplayAbilitySpec);
	}
}

float UAuraAbilitySystemLibrary::Sigmoid_Modify(const float X) {
	return (1.0f / (1.0f + exp(-X * 0.03f)) - 0.5f) * 2.f;
}

bool UAuraAbilitySystemLibrary::ProbabilityCheck(const float Probability) {
	return FMath::RandRange(0.f, 1.f) <= Probability;
}
