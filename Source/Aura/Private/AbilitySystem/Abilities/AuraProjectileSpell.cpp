// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"


FString UAuraProjectileSpell::GetDescription(int32 Level) const {
	FString DamageStr;
	for (auto& Pair : DamageTypes) {
		const int Damage = Pair.Value.Damage.GetValueAtLevel(Level);
		if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Fire)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Fire Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Lightning)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Lightning Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Arcane)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Arcane Damage!</>\n"), Damage));
		} else if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Physical)) {
			DamageStr.Append(FString::Printf(TEXT("<Damage>%d </><Default>Physical Damage!</>\n"), Damage));
		}
	}
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Level>LEVEL:%d</>\n\n<Small>ManaCost:%.1f</>\n\n<Small>Cooldown:%.1f</>\n\n<Default>Cause:</>\n%s"), Level, ManaCost, Cooldown, *DamageStr);
}

FString UAuraProjectileSpell::GetNextLevelDescription(int32 Level) const {
	return GetDescription(Level + 1);
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::Server_SpawnProjectile_Implementation(const FVector& ProjectileTargetLocation, const FGameplayTag& Tag) {
	check(ProjectileClass)
	check(DamageEffectClass)

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), Tag);
	const FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	//Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetAvatarActorFromActorInfo(), Cast<APawn>(GetAvatarActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(Projectile);

	Projectile->FinishSpawning(SpawnTransform);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& Tag) {
	if (GetAvatarActorFromActorInfo()->HasAuthority()) {
		Server_SpawnProjectile_Implementation(ProjectileTargetLocation, Tag);
	} else {
		Server_SpawnProjectile(ProjectileTargetLocation, Tag);
	}
}
