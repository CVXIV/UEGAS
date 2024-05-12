// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;

class UCameraComponent;

class UNiagaraComponent;

class AAuraPlayerState;

class UAuraPlayerAbilitySystemComponent;

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface {
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual int32 GetPlayerLevel() const override;

	virtual void OnRep_PlayerState() override;

	/* IPlayerInterface*/
	virtual void AddToXP(uint32 InXP) override;

	virtual void AddToAttributePoints(int32 InAttributePoints) override;

	virtual void AddToSpellPoints(int32 InSpellPoints) override;

	virtual uint32 FindLevelForXP(uint32 XP) override;

	virtual uint32 GetAttributePoints() const override;

	virtual uint32 GetSpellPoints() const override;

	/* IPlayerInterface*/

protected:
	void AddCharacterAbilities() const;

	UPROPERTY()
	TObjectPtr<UAuraPlayerAbilitySystemComponent> AuraPlayerAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<AAuraPlayerState> AuraPlayerState;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

private:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FAbilityDetail> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FAbilityDetail> StartupPassiveAbilities;
};
