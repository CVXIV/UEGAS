// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FAuraDataAssetAbilityInfoRow;

class UAuraAbilitySystemComponent;

class UAuraDataAssetAbilityInfo;

class UAuraUserWidget;

class UAuraAttributeSet;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

// 以F开头
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGiven, const FAuraDataAssetAbilityInfoRow&, AbilityInfo);

#define GAMEPLAY_ATTRIBUTE_ON_CHANGE(AttributeName) \
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->Get##AttributeName##Attribute()).AddLambda([this](const FOnAttributeChangeData& Data) { \
		On##AttributeName##Changed.Broadcast(Data.NewValue); \
	});

UCLASS(Blueprintable, BlueprintType)
class AURA_API UOverlayWidgetController : public UAuraWidgetController {
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	void OnAbilityGiven(const FGameplayAbilitySpec& AbilitySpec) const;

protected:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Abilitiy")
	FAbilityGiven AbilityGivenDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAuraDataAssetAbilityInfo> AbilityInfo;

	virtual void InitWidgetController() override;

	template <typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) {
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
