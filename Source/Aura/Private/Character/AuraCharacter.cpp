// Copyright Cvxiv


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter() {
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = true;
	CharacterMovementComponent->RotationRate = FRotator(0, 400.f, 0);
	CharacterMovementComponent->bConstrainToPlane = true;
	CharacterMovementComponent->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(false);
	
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	// server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

int32 AAuraCharacter::GetPlayerLevel() {
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo() {
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	check(AuraAbilitySystemComponent)
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
