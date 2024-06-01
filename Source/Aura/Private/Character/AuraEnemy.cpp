// Copyright Cvxiv
#include "Character/AuraEnemy.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy() {
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Weapon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AuraAbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AuraAbilitySystemComponent");
	AuraAbilitySystemComponent->SetIsReplicated(true);
	AuraAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 480, 0);
}

void AAuraEnemy::HighlightActor() {
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor() {
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel() const {
	return Level;
}

UAnimMontage* AAuraEnemy::GetHitReactMontage_Implementation() {
	return HitReactMontage;
}

void AAuraEnemy::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	if (!HasAuthority()) {
		return;
	}
	AuraAIController = Cast<AAuraAIController>(NewController);

	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool("HitReacting", false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool("Dead", false);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat("AttackRange", AttackRange);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget) {
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() {
	return CombatTarget;
}

void AAuraEnemy::BeginPlay() {
	Super::BeginPlay();

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetSimulatePhysics(false);
	Weapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AuraAbilitySystemComponent, CharacterClass);

	UAuraUserWidget* AuraUserWidget = CastChecked<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	AuraUserWidget->SetWidgetController(this);

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	// 客户端适配
	if (bHealthReplicated) {
		OnHealthInitialize.Broadcast(AuraAttributeSet->GetHealth());
		bHealthHasBroadcast = true;
	}

	// 客户端适配
	if (bMaxHealthReplicated) {
		OnMaxHealthInitialize.Broadcast(AuraAttributeSet->GetMaxHealth());
		bMaxHealthHasBroadcast = true;
	}

	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		if (bHealthHasBroadcast) {
			OnHealthChanged.Broadcast(Data.NewValue);
		} else {
			OnHealthInitialize.Broadcast(Data.NewValue);
			bHealthHasBroadcast = true;
		}
	});

	// 目前客户端对应的Data.NewValue为0（为BaseValue），服务器端正常（为CurrentValue），为兼容客户端，使用Data.Attribute.GetGameplayAttributeData(AttributeSet)->GetCurrentValue()
	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		if (bMaxHealthHasBroadcast) {
			OnMaxHealthChanged.Broadcast(Data.Attribute.GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
		} else {
			OnMaxHealthInitialize.Broadcast(Data.Attribute.GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
			bMaxHealthHasBroadcast = true;
		}
	});

	AuraAbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Action_HitReact, EGameplayTagEventType::NewOrRemoved).AddLambda([this](const FGameplayTag Tag, int32 NewCount) {
		bHitReacting = NewCount > 0;
		GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
		if (AuraAIController) {
			AuraAIController->GetBlackboardComponent()->SetValueAsBool("HitReacting", bHitReacting);
		}
	});

	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo() {
	Super::InitAbilityActorInfo();
	AuraAbilitySystemComponent->InitAbilityActorInfo(this, this);
	AuraAbilitySystemComponent->AbilityActorInfoSet();
	OnAscRegistered.Broadcast(AuraAbilitySystemComponent);
	
	InitializeDefaultAttributes();
}

void AAuraEnemy::OnDie() {
	Dissolve();
	if (AuraAIController) {
		AuraAIController->GetBlackboardComponent()->SetValueAsBool("Dead", true);
	}
	Super::OnDie();
}

void AAuraEnemy::InitializeDefaultAttributes() const {
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AuraAbilitySystemComponent);
}

void AAuraEnemy::Dissolve() {
	TArray<UMaterialInstanceDynamic*> MaterialInstanceDynamics;
	if (DissolveMaterial) {
		UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DissolveMaterial, this);
		GetMesh()->SetMaterial(0, MaterialInstanceDynamic);
		MaterialInstanceDynamics.Add(MaterialInstanceDynamic);
	}

	if (DissolveWeaponMaterial) {
		UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DissolveWeaponMaterial, this);
		Weapon->SetMaterial(0, MaterialInstanceDynamic);
		MaterialInstanceDynamics.Add(MaterialInstanceDynamic);
	}

	StartDissolve(MaterialInstanceDynamics);
}
