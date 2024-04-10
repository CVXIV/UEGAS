// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;

class UBehaviorTree;

class UWidgetComponent;

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface {
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void HighlightActor() override;

	virtual void UnHighlightActor() override;

	virtual int32 GetPlayerLevel() override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	virtual AActor* GetCombatTarget_Implementation() override;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	virtual void OnDie() override;

	virtual void InitializeDefaultAttributes() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolve(const TArray<UMaterialInstanceDynamic*>& MaterialInstanceDynamic);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthInitialize;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthInitialize;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackRange;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

private:
	void Dissolve();

	// Enemy的Level不需要网络传递，因为涉及到的关键计算只在服务器完成，没有必要传递到客户端
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true"))
	int32 Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECharacterClass> CharacterClass = Warrior;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> DissolveMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> DissolveWeaponMaterial;
};
