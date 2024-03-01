// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class UAttributeSet;

class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams {
	GENERATED_BODY()
	FWidgetControllerParams() {
	}

	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* Asc, UAttributeSet* As) : PlayerController(PC), PlayerState(PS), AbilitySystemComponent(Asc), AttributeSet(As) {
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

UCLASS(Abstract)
class AURA_API UAuraWidgetController : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WcParams);

	virtual void BroadcastInitialValues();

	virtual void BindCallbacksToDependencies();

protected:
	virtual void InitWidgetController();

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};
