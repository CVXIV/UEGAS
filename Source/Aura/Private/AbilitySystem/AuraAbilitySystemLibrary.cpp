// Copyright Cvxiv


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacter.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

const UAuraDataAssetAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContext) {
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext))) {
		return AuraGameModeBase->AbilityInfo;
	}

	if (const AAuraCharacter* Character = Cast<AAuraCharacter>(WorldContext)) {
		return Character->GetAbilityInfo();
	}
	UE_LOG(LogTemp, Error, TEXT("未找到UAuraDataAssetAbilityInfo"))
	return nullptr;
}

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

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContext) {
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContext, 0)) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD())) {
			AAuraPlayerState* PS = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();
			const FWidgetControllerParams Params(PlayerController, PS, Asc, As);
			return AuraHUD->GetSpellMenuWidgetController(Params);
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

bool UAuraAbilitySystemLibrary::IsTakeHitReact(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->IsTakeHitReact();
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

void UAuraAbilitySystemLibrary::SetTakeHitReact(FGameplayEffectContextHandle& EffectContextHandle, bool bInTakeHitReact) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetTakeHitReact(bInTakeHitReact);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetDeathImpulse(InDeathImpulse);
	}
}

const FVector& UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetKnockBackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackForce) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetKnockBackForce(InKnockBackForce);
	}
}

const FVector& UAuraAbilitySystemLibrary::GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetKnockBackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->IsRadialDamage();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetIsRadialDamage(bInIsRadialDamage);
	}
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetRadialDamageInnerRadius();
	}
	return 0;
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageInnerRadius) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetRadiusDamageOuterRadius();
	}
	return 0;
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageOuterRadius) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->SetRadiusDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

const FVector& UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle) {
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

const TArray<FDeBuffProperty>& UAuraAbilitySystemLibrary::GetDeBuffProperty(const FGameplayEffectContextHandle& EffectContextHandle) {
	static TArray<FDeBuffProperty> Empty;
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		return Context->GetDeBuffProperties();
	}
	return Empty;
}

void UAuraAbilitySystemLibrary::SetDeBuffProperty(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDeBuff, const FGameplayTag& InDeBuff, float InDamage, float InFrequency, float InDuration) {
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
		Context->AddDeBuffInfo(bInIsSuccessfulDeBuff, InDeBuff, InDamage, InFrequency, InDuration);
	}
}

float UAuraAbilitySystemLibrary::CalcRadialDamageWithFalloff(float BaseDamage, float DistanceFromEpicenter, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff) {
	float const ValidatedInnerRadius = FMath::Max(0.f, DamageInnerRadius);
	float const ValidatedOuterRadius = FMath::Max(DamageOuterRadius, ValidatedInnerRadius);
	float const ValidatedDist = FMath::Max(0.f, DistanceFromEpicenter);

	if (ValidatedDist >= ValidatedOuterRadius) {
		// outside the radius, no effect
		return 0.f;
	}

	if ((DamageFalloff == 0.f) || (ValidatedDist <= ValidatedInnerRadius)) {
		// no falloff or inside inner radius means full effect
		return BaseDamage;
	}

	// calculate the interpolated scale
	float DamageScale = 1.f - ((ValidatedDist - ValidatedInnerRadius) / (ValidatedOuterRadius - ValidatedInnerRadius));
	DamageScale = FMath::Pow(DamageScale, DamageFalloff);

	return BaseDamage * DamageScale;
}

TArray<AActor*> UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContext, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin) {
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TSet<AActor*> OverlappingActors;
	// query scene to see what we hit
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull)) {
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

		for (FOverlapResult& Overlap : Overlaps) {
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()) && FVector::Distance(SphereOrigin, Overlap.GetActor()->GetActorLocation()) <= Radius) {
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyGameplayEffect(FDamageEffectParams& DamageEffectParams) {
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor());
	const FGameplayEffectSpecHandle EffectSpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	for (int i = 0; i < DamageEffectParams.DamageTypesKeys.Num(); ++i) {
		const FGameplayTag& Tag = DamageEffectParams.DamageTypesKeys[i];
		const FDamageTypeInfo& DamageTypeInfo = DamageEffectParams.DamageTypesValues[i];

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tag, DamageTypeInfo.Damage.GetValueAtLevel(DamageEffectParams.AbilityLevel));

		const FGameplayTag& DeBuff_Chance = AuraGameplayTags.DamageTypesToDeBuffAndResistance[Tag].DeBuff_Chance;
		const FGameplayTag& DeBuff_Damage = AuraGameplayTags.DamageTypesToDeBuffAndResistance[Tag].DeBuff_Damage;
		const FGameplayTag& DeBuff_Frequency = AuraGameplayTags.DamageTypesToDeBuffAndResistance[Tag].DeBuff_Frequency;
		const FGameplayTag& DeBuff_Duration = AuraGameplayTags.DamageTypesToDeBuffAndResistance[Tag].DeBuff_Duration;
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DeBuff_Chance, DamageTypeInfo.DeBuffChance);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DeBuff_Damage, DamageTypeInfo.DeBuffDamage);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DeBuff_Frequency, DamageTypeInfo.DeBuffFrequency);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DeBuff_Duration, DamageTypeInfo.DeBuffDuration);
	}

	// TODO 待优化
	SetKnockBackForce(EffectContextHandle, DamageEffectParams.Instigator->GetActorForwardVector() * DamageEffectParams.KnockBackForceMagnitude);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.Instigator->GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude);
	
	SetTakeHitReact(EffectContextHandle, DamageEffectParams.bTakeHitReact);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadiusDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	return EffectContextHandle;
}

const FGameplayTag& UAuraAbilitySystemLibrary::GetAbilityTagFromAbility(const UGameplayAbility* GameplayAbility) {
	for (const FGameplayTag& GameplayTag : GameplayAbility->AbilityTags) {
		if (GameplayTag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability"))) {
			return GameplayTag;
		}
	}
	return FAuraGameplayTags::Get().None;
}

FGameplayTag UAuraAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec) {
	for (const FGameplayTag& GameplayTag : AbilitySpec.DynamicAbilityTags) {
		if (GameplayTag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag"))) {
			return GameplayTag;
		}
	}
	return FGameplayTag();
}

EAbilityStatus UAuraAbilitySystemLibrary::GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec) {
	for (const FGameplayTag& GameplayTag : AbilitySpec.DynamicAbilityTags) {
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Locked)) {
			return EAbilityStatus::Locked;
		}
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Eligible)) {
			return EAbilityStatus::Eligible;
		}
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Unlocked)) {
			return EAbilityStatus::Unlocked;
		}
		if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Equipped)) {
			return EAbilityStatus::Equipped;
		}
	}
	return EAbilityStatus::None;
}

bool UAuraAbilitySystemLibrary::IsTagValid(const FGameplayTag& Tag) {
	return Tag.IsValid() && !FAuraGameplayTags::Get().None.MatchesTagExact(Tag);
}
