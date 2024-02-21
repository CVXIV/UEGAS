// Copyright Cvxiv

#include "Character/AuraCharacterBase.h"

AAuraCharacterBase::AAuraCharacterBase() {
    PrimaryActorTick.bCanEverTick = false;

    m_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
    m_Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    m_Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay() {
    Super::BeginPlay();
}
