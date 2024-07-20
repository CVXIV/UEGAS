// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Data/AuraDataAssetAbilityInfo.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FDamageEffectParams;

class UAuraDataAssetAbilityInfo;

class USpellMenuWidgetController;

struct FAuraGameplayEffectSpecHandle;

struct FGameplayEffectContextHandle;

class UAbilitySystemComponent;

class UOverlayWidgetController;

class UAttributeMenuWidgetController;

UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	static const UAuraDataAssetAbilityInfo* GetAbilityInfo(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContext"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContext"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContext"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContext, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContext, UAbilitySystemComponent* AbilitySystemComponent, ECharacterClass CharacterClass);

	/**
	 * 返回[0,1)的一个值，其中X>=0
	 * @param X 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Math")
	static float Sigmoid_Modify(const float X);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Math")
	static bool ProbabilityCheck(const float Probability);

	/*
	 * GameplayContext Setter Getter
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsTakeHitReact(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetTakeHitReact(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInTakeHitReact);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static const FVector& GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetKnockBackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackForce);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static const FVector& GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageInnerRadius);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageOuterRadius);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static const FVector& GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin);

	/*
	 * Handy Func
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<AActor*> GetLivePlayersWithinRadius(const UObject* WorldContext, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool TryActivateRandomAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|XP")
	static float GetXPForCharacterClassAndLevel(const UObject* WorldContext, ECharacterClass CharacterClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyGameplayEffect(UPARAM(ref) FDamageEffectParams& DamageEffectParams);

	static const FGameplayTag& GetAbilityTagFromAbility(const UGameplayAbility* GameplayAbility);

	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	static EAbilityStatus GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	static bool IsTagValid(const FGameplayTag& Tag);

	static const TArray<FDeBuffProperty>& GetDeBuffProperty(const FGameplayEffectContextHandle& EffectContextHandle);

	static void SetDeBuffProperty(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDeBuff, const FGameplayTag& InDeBuff, float InDamage, float InFrequency, float InDuration);

	static float CalcRadialDamageWithFalloff(float BaseDamage, float DistanceFromEpicenter, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff);
};
