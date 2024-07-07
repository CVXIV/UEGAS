// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;

UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility {
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) const override;

	virtual FString GetNextLevelDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, AActor* ProjectileTarget, const FGameplayTag& Tag);

	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(const FVector& ProjectileTargetLocation, AActor* ProjectileTarget, const FGameplayTag& Tag);

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1), Category = "Projectile")
	int32 MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Projectile")
	float HomingAccelerationMagnitudeMin = 2000.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Projectile")
	float HomingAccelerationMagnitudeMax = 5000.f;

private:
	void SpawnProjectile(const FVector& TargetLocation, const FVector& SpawnedLocation, AActor* ProjectileTarget, const FVector& Direction) const;
};
