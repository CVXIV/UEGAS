// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraArcaneShardsAbility.h"

#include "AuraGameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/PointCollection.h"

UAbilityTask_SpawnArcaneShards* UAbilityTask_SpawnArcaneShards::SpawnArcaneShardsAndTakeDamage(UGameplayAbility* OwningAbility, const TArray<FVector>& Locations) {
	UAbilityTask_SpawnArcaneShards* MyTask = NewAbilityTask<UAbilityTask_SpawnArcaneShards>(OwningAbility);
	MyTask->ArcaneShardsAbility = CastChecked<UAuraDamageGameplayAbility>(OwningAbility);
	MyTask->SpawnLocations = Locations;
	MyTask->CurrentLoop = 0;
	return MyTask;
}

void UAbilityTask_SpawnArcaneShards::Activate() {
	if (Ability == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Empty Ability!"))
		return;
	}
	if (CurrentLoop >= SpawnLocations.Num()) {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnSuccess.Broadcast();
		}
	} else {
		InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_SpawnArcaneShards::OnPreDestroy);
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]() {
			SpawnArcaneShards_Internal(SpawnLocations[CurrentLoop]);
			CurrentLoop++;
			if (CurrentLoop == SpawnLocations.Num()) {
				if (ShouldBroadcastAbilityTaskDelegates()) {
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
					OnSuccess.Broadcast();
				}
			}
		}), 0.2f, true, 0);
	}
}

void UAbilityTask_SpawnArcaneShards::OnDestroy(bool bInOwnerFinished) {
	if (Ability) {
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
	}

	OnPreDestroy();
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_SpawnArcaneShards::OnPreDestroy() {
}

void UAbilityTask_SpawnArcaneShards::SpawnArcaneShards_Internal(const FVector& Location) const {
	// 注意！这里客户端和服务器生成的随机Location不一样，客户端只是进行预测；但是最终显示的GC是一样的，因为服务器会把权威的GC发送到客户端
	FGameplayCueParameters CueParameters;
	CueParameters.Location = Location;
	// 优化带宽可以使用ExecuteGameplayCue_NonReplicated
	//UGameplayCueManager::ExecuteGameplayCue_NonReplicated()
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(Ability->GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().Cue_ArcaneShards, CueParameters);
	// 计算伤害
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Ability->GetAvatarActorFromActorInfo());
	TArray<AActor*> TargetActors = UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(Ability->GetAvatarActorFromActorInfo(), ActorsToIgnore, ArcaneShardsAbility->GetRadiusDamageOuterRadius(), Location);
	for (AActor* Target : TargetActors) {
		FDamageEffectParams DamageEffectParams = ArcaneShardsAbility->MakeDamageEffectParamsFromClassDefaults(Ability->GetAvatarActorFromActorInfo(), Target, Location);
		UAuraAbilitySystemLibrary::ApplyGameplayEffect(DamageEffectParams);
	}
}

TArray<FVector> UAuraArcaneShardsAbility::GetRandomSpawnLocations(const FVector& Location) {
	check(PointCollectionClass)
	if (!PointCollection) {
		PointCollection = GetWorld()->SpawnActor<APointCollection>(PointCollectionClass, Location, FRotator::ZeroRotator);
	} else {
		PointCollection->SetActorLocation(Location);
	}
	return PointCollection->GetGroundPoints(Location, FMath::Max(1, GetAbilityLevel()), FMath::RandRange(0, 360));
}
