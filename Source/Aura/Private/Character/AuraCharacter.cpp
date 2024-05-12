// Copyright Cvxiv


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"
#include "AbilitySystem/Data/AuraDataAssetLevelUpInfo.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter() {
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);

	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = true;
	CharacterMovementComponent->RotationRate = FRotator(0, 400.f, 0);
	CharacterMovementComponent->bConstrainToPlane = true;
	CharacterMovementComponent->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

void AAuraCharacter::BeginPlay() {
	Super::BeginPlay();

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	// server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

int32 AAuraCharacter::GetPlayerLevel() const {
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP(uint32 InXP) {
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::AddToAttributePoints(int32 InAttributePoints) {
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints(int32 InSpellPoints) {
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

uint32 AAuraCharacter::FindLevelForXP(uint32 XP) {
	return AuraPlayerState->DataAssetLevelUpInfo->FindLevelForXp(XP);
}

uint32 AAuraCharacter::GetAttributePoints() const {
	return AuraPlayerState->GetAttributePoints();
}

uint32 AAuraCharacter::GetSpellPoints() const {
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::AddCharacterAbilities() const {
	check(IsValid(AuraPlayerAbilitySystemComponent))
	if (!HasAuthority()) { return; }

	AuraPlayerAbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
	AuraPlayerAbilitySystemComponent->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacter::InitAbilityActorInfo() {
	AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->OnLevelChangedDelegate.AddLambda([this](uint32, uint32) {
		const FVector CameraLoc = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraLoc = LevelUpNiagaraComponent->GetComponentLocation();
		LevelUpNiagaraComponent->SetWorldRotation((CameraLoc - NiagaraLoc).Rotation());
		LevelUpNiagaraComponent->Activate(true);
	});

	AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	check(AuraAbilitySystemComponent)

	AuraPlayerAbilitySystemComponent = Cast<UAuraPlayerAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	check(AuraPlayerAbilitySystemComponent)

	AuraAbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	AuraAbilitySystemComponent->AbilityActorInfoSet();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	// 这里不用断言，考虑到多人情况
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController())) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD())) {
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AuraAbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}
