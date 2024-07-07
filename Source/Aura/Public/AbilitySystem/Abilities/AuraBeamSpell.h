// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

USTRUCT()
struct FTempActorInfo {
	GENERATED_BODY()
	FTempActorInfo() {
	}

	explicit FTempActorInfo(const FDamageEffectParams& InDamageEffectParams) {
		this->EffectParams = InDamageEffectParams;
	}

	explicit FTempActorInfo(const FGameplayCueParameters& InCueParameters) {
		this->CueParameters = InCueParameters;
	}

	explicit FTempActorInfo(FDelegateHandle InDelegateHandle) {
		this->DelegateHandle = InDelegateHandle;
	}

	UPROPERTY()
	FGameplayCueParameters CueParameters;

	UPROPERTY()
	FDamageEffectParams EffectParams;

	FDelegateHandle DelegateHandle;
};

class AAuraCharacterBase;

UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility {
	GENERATED_BODY()

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerController();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void SpawnElectricBeamWithRange();

	UFUNCTION(BlueprintCallable)
	void ApplyDamageOnce();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAbilityEnd();

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	FGameplayTag CueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	uint32 AttachRange;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxBeam;

private:
	void SpawnElectricBeam(AActor* TargetActor, USceneComponent* AttachComponent, const FVector& TargetLocation);

	void AddInfoToToApplyActorInfo(AActor* Target, const FGameplayCueParameters& CueParameters);

	void AddInfoToToApplyActorInfo(AActor* Target, const FDamageEffectParams& EffectParams);

	void AddInfoToToApplyActorInfo(AActor* Target, const FDelegateHandle& DelegateHandle);

	UPROPERTY()
	TMap<AActor*, FTempActorInfo> ToApplyActorInfo;
};
