// Copyright Cvxiv

#include "Character/AuraCharacterBase.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraCharacterBase::AAuraCharacterBase() {
	PrimaryActorTick.bCanEverTick = false;
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetGenerateOverlapEvents(false);
}

void AAuraCharacterBase::BeginPlay() {
	Super::BeginPlay();

	bDead = false;
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const {
	return AuraAbilitySystemComponent;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) {
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(AuraGameplayTags.Montage_Attack_Weapon)) {
		check(Weapon)
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(AuraGameplayTags.Montage_Attack_LeftHand)) {
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(AuraGameplayTags.Montage_Attack_RightHand)) {
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(AuraGameplayTags.Montage_Attack_Tail)) {
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

bool AAuraCharacterBase::IsDead_Implementation() const {
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation() {
	return this;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation() const {
	return CharacterClass;
}

FTaggedMontage AAuraCharacterBase::GetAttackMontage_Random_Implementation() const {
	check(!AttackMontage.IsEmpty())
	const int Index = FMath::RandRange(0, AttackMontage.Num() - 1);
	return AttackMontage[Index];
}

FTaggedMontage AAuraCharacterBase::GetAttackMontageByTag_Implementation(const FGameplayTag& Tag) const {
	check(!AttackMontage.IsEmpty())
	for (const FTaggedMontage& Item : AttackMontage) {
		if (Item.MontageTag.MatchesTagExact(Tag)) {
			return Item;
		}
	}
	checkf(false, TEXT("invalid tag:%s"), *Tag.ToString())
	return FTaggedMontage();
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation() const {
	return BloodEffect;
}

void AAuraCharacterBase::Die_Implementation() {
	OnDie();
}

void AAuraCharacterBase::OnDie() {
	bDead = true;

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::InitAbilityActorInfo() {
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffect, const float Level) const {
	check(IsValid(AuraAbilitySystemComponent))
	check(GameplayEffect)
	FGameplayEffectContextHandle GameplayEffectContextHandle = AuraAbilitySystemComponent->MakeEffectContext();
	GameplayEffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = AuraAbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, Level, GameplayEffectContextHandle);
	AuraAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), AuraAbilitySystemComponent);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const {
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}
