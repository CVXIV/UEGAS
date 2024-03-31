// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;

class UOverlayWidgetController;

class UAttributeMenuWidgetController;

UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContext, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContext, UAbilitySystemComponent* AbilitySystemComponent);

	/**
	 * 返回[0,1)的一个值，其中X>=0
	 * @param X 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Math")
	static float Sigmoid_Modify(const float X);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Math")
	static bool ProbabilityCheck(const float Probability);
};
