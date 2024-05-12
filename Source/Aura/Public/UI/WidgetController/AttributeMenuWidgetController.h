// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;

class UAuraAttributeSet;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagAttributeChangedSignature, const FAuraAttributeInfo&, NewAttributeInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature, int32, NewValue);

UCLASS(Blueprintable, BlueprintType)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController {
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnTagAttributeChangedSignature OnAttributeChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStateChangedSignature AttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	virtual void InitWidgetController() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
