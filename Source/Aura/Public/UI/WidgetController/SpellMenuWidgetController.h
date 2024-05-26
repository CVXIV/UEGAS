// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

UENUM(BlueprintType)
enum class EAbilityType:uint8 {
	None,
	Offensive,
	Passive
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, Description, FString, NextLevelDescription);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSignature, EAbilityType, AbilityType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndEquipSignature);

class UAttributeInfo;

UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController {
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature Dd_SpellPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature Dd_SpellGlobeSelected;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSignature Dd_WaitForEquip;

	UPROPERTY(BlueprintAssignable)
	FEndEquipSignature Dd_EndEquip;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpellEquipSelected();

	virtual void OnAbilityChange(const FGameplayAbilitySpec& AbilitySpec) override;

	UFUNCTION(BlueprintCallable)
	void SpendPointsButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnSpellMenuClosed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobeClicked(const FGameplayTag& SlotTag);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Slot, const FGameplayTag& PreSlot);

private:
	FGameplayTag SelectedAbilityTag = FGameplayTag();

	FGameplayTag SelectedSlot = FGameplayTag();

	bool bWaitForEquipSelection = false;
};
