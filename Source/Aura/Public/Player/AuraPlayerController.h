// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;

class UAuraAbilitySystemComponent;

class UAuraInputConfig;

class UInputMappingContext;

class UInputAction;

class IEnemyInterface;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ClientShowWidget(AActor* Target, float Damage, bool bIsCriticalHit, bool bIsBlockedHit);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> FloatTextWidgetClass;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 声明类型，也可以写在前面
	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();

	void AutoRunToDestination();

	IEnemyInterface* LastActor;

	IEnemyInterface* ThisActor;

	void AbilityInputTagPressed(const FGameplayTag InputTag);

	void AbilityInputTagReleased(const FGameplayTag InputTag);

	void AbilityInputTagHeld(const FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	FVector CachedDestination = FVector::ZeroVector;

	float FollowTime = 0.f;

	float ShortPressedThreshold = 0.5f;

	bool bTargeting = false;

	bool bAutoRunning = false;

	FHitResult HitResult;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 30.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
};
