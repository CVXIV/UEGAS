// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;

class UProjectileMovementComponent;

class USphereComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor {
	GENERATED_BODY()

public:
	AAuraProjectile();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> LoopSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopSoundComponent;

private:
	void OnOverlap() const;

	uint8 bIsOverlap : 1;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDestroy();
};
