// Copyright Cvxiv


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraProjectile::AAuraProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(SphereComponent);

	SphereComponent->SetCollisionObjectType(ECC_PROJECTILE);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraProjectile, DamageEffectParams)
}

void AAuraProjectile::BeginPlay() {
	Super::BeginPlay();

	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopSound, GetRootComponent());
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
}

void AAuraProjectile::Destroyed() {
	if (bIsOverlap) {
		OnOverlap();
	}
	if (LoopSoundComponent) {
		LoopSoundComponent->Stop();
		LoopSoundComponent->DestroyComponent();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const AActor* AvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (OtherActor == AvatarActor) {
		return;
	}
	if (HasAuthority()) {
		if (UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)) {
			DamageEffectParams.TargetAbilitySystemComponent = Asc;
			UAuraAbilitySystemLibrary::ApplyGameplayEffect(DamageEffectParams);
		}
		Multicast_OnDestroy();
	}
}

void AAuraProjectile::OnOverlap() const {
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}

void AAuraProjectile::OnDestroy(bool bNaturalDied) {
	if (HasAuthority()) {
		bIsOverlap = !bNaturalDied;
		Destroy();
	}
}

void AAuraProjectile::Multicast_OnDestroy_Implementation() {
	OnDestroy(false);
}
