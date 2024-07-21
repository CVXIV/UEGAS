// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::Server_SpawnProjectile_Implementation(const FVector& ProjectileTargetLocation, AActor* ProjectileTarget, const FGameplayTag& Tag) {
	check(ProjectileClass)
	check(DamageEffectClass)

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), Tag);
	const FVector Forward = ProjectileTargetLocation - SocketLocation;
	//Rotation.Pitch = 0.f;

	const int32 NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	if (NumProjectiles == 1) {
		SpawnProjectile(ProjectileTargetLocation, SocketLocation, ProjectileTarget, Forward);
	} else {
		const float DeltaSpread = ProjectileSpread / (NumProjectiles - 1);
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread * 0.5f, FVector::UpVector);
		for (int32 i = 0; i < NumProjectiles; ++i) {
			const FVector Direction = LeftOfSpread.RotateAngleAxis(i * DeltaSpread, FVector::UpVector);
			SpawnProjectile(ProjectileTargetLocation, SocketLocation, ProjectileTarget, Direction);
		}
	}
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, AActor* ProjectileTarget, const FGameplayTag& Tag) {
	if (GetAvatarActorFromActorInfo()->HasAuthority()) {
		Server_SpawnProjectile_Implementation(ProjectileTargetLocation, ProjectileTarget, Tag);
	} else {
		Server_SpawnProjectile(ProjectileTargetLocation, ProjectileTarget, Tag);
	}
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation, const FVector& SpawnedLocation, AActor* ProjectileTarget, const FVector& Direction) const {
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnedLocation);
	SpawnTransform.SetRotation(Direction.Rotation().Add(45, 0, 0).Quaternion());

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetAvatarActorFromActorInfo(), Cast<APawn>(GetAvatarActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(Projectile);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ProjectileTarget)) {
		if (!ICombatInterface::Execute_IsDead(ProjectileTarget)) {
			Projectile->ProjectileMovementComponent->HomingTargetComponent = ProjectileTarget->GetRootComponent();
			CombatInterface->GetOnDiedDelegate().AddWeakLambda(Projectile, [Projectile](const AAuraCharacterBase* Target) {
				if (IsValid(Projectile)) {
					Projectile->ProjectileMovementComponent->bIsHomingProjectile = false;
				}
			});
		}
	} else {
		Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
		Projectile->HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
		Projectile->ProjectileMovementComponent->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
	}
	Projectile->ProjectileMovementComponent->bIsHomingProjectile = true;
	Projectile->ProjectileMovementComponent->ProjectileGravityScale = 0.3f;
	Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMagnitudeMin, HomingAccelerationMagnitudeMax);

	Projectile->FinishSpawning(SpawnTransform);
}
