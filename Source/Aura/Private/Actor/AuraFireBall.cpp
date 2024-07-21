// Copyright Cvxiv


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireBall::BeginPlay() {
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const AActor* AvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (OtherActor == AvatarActor) {
		return;
	}
	if (HasAuthority()) {
		if (UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)) {
			DamageEffectParams.TargetAbilitySystemComponent = Asc;
			UAuraAbilitySystemLibrary::ApplyGameplayEffect(DamageEffectParams);
			OnOverlap();
		}
	}
}
