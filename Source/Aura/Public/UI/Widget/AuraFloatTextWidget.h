// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraFloatTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFloatTextWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageText(float Damage, bool bIsCriticalHit, bool bIsBlockedHit);

	UPROPERTY()
	AActor* OwningActor;

	FVector2D BaseOffset = FVector2D::Zero();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;
};
