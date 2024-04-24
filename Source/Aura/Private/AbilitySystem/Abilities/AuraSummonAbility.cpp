// Copyright Cvxiv


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "NiagaraFunctionLibrary.h"
#include "Algo/RandomShuffle.h"
#include "Character/AuraCharacterBase.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations() const {
	check(NumMinions > 0)
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	TArray<FVector> SpawnLocations;
	if (NumMinions == 1) {
		const FVector SpreadDirection = Forward.RotateAngleAxis(SpawnSpread * FMath::FRandRange(-0.5f, 0.5f), FVector::UpVector);
		FVector ChosenSpawnLocation = Location + SpreadDirection * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0, 0, 400.f), ChosenSpawnLocation - FVector(0, 0, 400.f), ECC_Visibility);
		if (HitResult.bBlockingHit) {
			ChosenSpawnLocation = HitResult.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	} else {
		const float DeltaSpread = SpawnSpread / (NumMinions - 1);
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread * 0.5f, FVector::UpVector);
		for (uint32 i = 0; i < NumMinions; ++i) {
			const FVector Direction = LeftOfSpread.RotateAngleAxis(i * DeltaSpread, FVector::UpVector);
			FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0, 0, 400.f), ChosenSpawnLocation - FVector(0, 0, 400.f), ECC_Visibility);
			if (HitResult.bBlockingHit) {
				ChosenSpawnLocation = HitResult.ImpactPoint;
			}
			SpawnLocations.Add(ChosenSpawnLocation);
		}
	}

	return SpawnLocations;
}

void UAuraSummonAbility::SpawnMinion_Internal(const FVector& SpawnLocation) {
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnTipNiagara, SpawnLocation);
	const TSubclassOf<AAuraCharacterBase>& SpawnClass = MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAuraCharacterBase* Pawn = GetWorld()->SpawnActor<AAuraCharacterBase>(SpawnClass, FVector(SpawnLocation.X, SpawnLocation.Y, GetAvatarActorFromActorInfo()->GetActorLocation().Z), FRotator::ZeroRotator, SpawnParameters);
	Pawn->SpawnDefaultController();
}

void UAuraSummonAbility::SpawnMinion() {
	check(!MinionClasses.IsEmpty())

	TArray<FVector> SpawnLocations = GetSpawnLocations();
	Algo::RandomShuffle(SpawnLocations);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	for (int i = 0; i < SpawnLocations.Num(); ++i) {
		const FVector& SpawnLocation = SpawnLocations[i];

		if (i == 0) {
			SpawnMinion_Internal(SpawnLocation);
		} else {
			FTimerHandle TimerHandle;
			TimerManager.SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, SpawnLocation]() {
				SpawnMinion_Internal(SpawnLocation);
			}), i * 0.5f, false);
		}
	}
}
