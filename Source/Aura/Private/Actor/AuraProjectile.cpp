// Copyright Cvxiv


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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

void AAuraProjectile::BeginPlay() {
	Super::BeginPlay();

	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopSound, GetRootComponent());
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
}

void AAuraProjectile::Destroyed() {
	// 只有不是服务器且bIsHit为false，才说明是服务器先执行的Overlap
	if (bIsOverlap && !bIsHit && !HasAuthority()) {
		// 说明是服务器先Destroy，客户端还没来得及渲染
		OnOverlap();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == GetOwner()) {
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	OnOverlap();
	bIsOverlap = true;
	if (HasAuthority()) {
		if (UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)) {
			Asc->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data);
		}
		Multicast_OnDestroy();
	} else {
		bIsHit = true;
	}
}

void AAuraProjectile::OnOverlap() const {
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopSoundComponent) {
		LoopSoundComponent->Stop();
	}
}

void AAuraProjectile::Multicast_OnDestroy_Implementation() {
	bIsOverlap = true;
	Destroy();
}
