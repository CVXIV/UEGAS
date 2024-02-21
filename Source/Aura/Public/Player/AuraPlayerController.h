// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerController: public APlayerController {
    GENERATED_BODY()

public:
    AAuraPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY(EditAnywhere, Category = "Input", DisplayName = "AuraContext")
    TObjectPtr<UInputMappingContext> m_AuraContext;
    UPROPERTY(EditAnywhere, Category = "Input", DisplayName = "MoveAction")
    TObjectPtr<UInputAction> m_MoveAction;

    // 声明类型，也可以写在前面
    void Move(const struct FInputActionValue& InputActionValue);
};
