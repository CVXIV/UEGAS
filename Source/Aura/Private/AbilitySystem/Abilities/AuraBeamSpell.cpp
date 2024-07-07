// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameplayCueFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) {
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	ToApplyActorInfo.Empty();
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult) {
	if (HitResult.bBlockingHit) {
		MouseHitActor = HitResult.GetActor();
		if (MouseHitActor->Implements<UCombatInterface>()) {
			MouseHitLocation = MouseHitActor->GetActorLocation();
		} else {
			MouseHitLocation = HitResult.ImpactPoint;
		}
	} else {
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerPlayerController() {
	if (CurrentActorInfo) {
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = OwnerPlayerController->GetCharacter();
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation) {
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerCharacter)) {
		if (const USkeletalMeshComponent* Weapon = CombatInterface->GetWeapon()) {
			const FVector SocketLocation = Weapon->GetSocketLocation("TipSocket");
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(GetOwningActorFromActorInfo());
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(GetOwningActorFromActorInfo(), SocketLocation, BeamTargetLocation, 1.f, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (HitResult.bBlockingHit) {
				MouseHitActor = HitResult.GetActor();
				if (MouseHitActor->Implements<UCombatInterface>()) {
					MouseHitLocation = MouseHitActor->GetActorLocation();
				} else {
					MouseHitLocation = HitResult.ImpactPoint;
				}
			}
		}
	}
}

void UAuraBeamSpell::SpawnElectricBeamWithRange() {
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerCharacter)) {
		bool bSpawnBeam;
		if (MouseHitActor->Implements<UCombatInterface>()) {
			bSpawnBeam = !ICombatInterface::Execute_IsDead(MouseHitActor);

			if (bSpawnBeam) {
				const FDelegateHandle MainActorDelegateHandle = Cast<ICombatInterface>(MouseHitActor)->GetOnDiedDelegate().AddWeakLambda(this, [this](const AAuraCharacterBase* Target) {
					OnAbilityEnd();
				});
				AddInfoToToApplyActorInfo(MouseHitActor, MainActorDelegateHandle);
				AddInfoToToApplyActorInfo(MouseHitActor, MakeDamageEffectParamsFromClassDefaults(GetAvatarActorFromActorInfo(), MouseHitActor));

				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
				ActorsToIgnore.Add(MouseHitActor);
				TArray<AActor*> Actors = UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(GetOwningActorFromActorInfo(), ActorsToIgnore, AttachRange, MouseHitActor->GetActorLocation());

				const int CurrentBeam = FMath::Min(MaxBeam, GetAbilityLevel());
				if (Actors.Num() > CurrentBeam) {
					Actors.Sort([this](const AActor& Actor1, const AActor& Actor2) {
						return FVector::DistSquared(Actor1.GetActorLocation(), MouseHitLocation) < FVector::DistSquared(Actor2.GetActorLocation(), MouseHitLocation);
					});
				}
				for (int i = 0; i < FMath::Min(CurrentBeam, Actors.Num()); ++i) {
					AActor* Actor = Actors[i];
					SpawnElectricBeam(Actor, MouseHitActor->GetRootComponent(), MouseHitActor->GetActorLocation());

					AddInfoToToApplyActorInfo(Actor, MakeDamageEffectParamsFromClassDefaults(GetAvatarActorFromActorInfo(), Actor));
					FDelegateHandle DelegateHandle = Cast<ICombatInterface>(Actor)->GetOnDiedDelegate().AddWeakLambda(this, [this, Actor](const AAuraCharacterBase* Target) {
						UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(Actor, CueTag, ToApplyActorInfo[Actor].CueParameters);
						ToApplyActorInfo.Remove(Actor);
					});
					AddInfoToToApplyActorInfo(Actor, DelegateHandle);
				}
			}
		} else {
			bSpawnBeam = true;
		}

		if (bSpawnBeam) {
			if (USkeletalMeshComponent* Weapon = CombatInterface->GetWeapon()) {
				SpawnElectricBeam(MouseHitActor, Weapon, MouseHitLocation);
			}
		}
	}
}

void UAuraBeamSpell::ApplyDamageOnce() {
	auto It = ToApplyActorInfo.CreateIterator();
	while (It) {
		if (IsValid(It.Key()) && It.Key()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(It.Key())) {
			UAuraAbilitySystemLibrary::ApplyGameplayEffect(It.Value().EffectParams);
		}
		++It;
	}
}

void UAuraBeamSpell::OnAbilityEnd_Implementation() {
	for (const TTuple<AActor*, FTempActorInfo>& Pair : ToApplyActorInfo) {
		UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(Pair.Key, CueTag, Pair.Value.CueParameters);
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Pair.Key)) {
			CombatInterface->GetOnDiedDelegate().Remove(Pair.Value.DelegateHandle);
		}
	}
}

void UAuraBeamSpell::SpawnElectricBeam(AActor* TargetActor, USceneComponent* AttachComponent, const FVector& TargetLocation) {
	FGameplayCueParameters CueParameters;
	// 用作更新位置
	CueParameters.Instigator = TargetActor;
	CueParameters.SourceObject = TargetActor;
	CueParameters.Location = TargetLocation;
	CueParameters.TargetAttachComponent = AttachComponent;
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(TargetActor, CueTag, CueParameters);

	AddInfoToToApplyActorInfo(TargetActor, CueParameters);
}

void UAuraBeamSpell::AddInfoToToApplyActorInfo(AActor* Target, const FGameplayCueParameters& CueParameters) {
	if (FTempActorInfo* TempActorInfo = ToApplyActorInfo.Find(Target)) {
		TempActorInfo->CueParameters = CueParameters;
	} else {
		ToApplyActorInfo.Add(Target, FTempActorInfo(CueParameters));
	}
}

void UAuraBeamSpell::AddInfoToToApplyActorInfo(AActor* Target, const FDamageEffectParams& EffectParams) {
	if (FTempActorInfo* TempActorInfo = ToApplyActorInfo.Find(Target)) {
		TempActorInfo->EffectParams = EffectParams;
	} else {
		ToApplyActorInfo.Add(Target, FTempActorInfo(EffectParams));
	}
}

void UAuraBeamSpell::AddInfoToToApplyActorInfo(AActor* Target, const FDelegateHandle& DelegateHandle) {
	if (FTempActorInfo* TempActorInfo = ToApplyActorInfo.Find(Target)) {
		TempActorInfo->DelegateHandle = DelegateHandle;
	} else {
		ToApplyActorInfo.Add(Target, FTempActorInfo(DelegateHandle));
	}
}
