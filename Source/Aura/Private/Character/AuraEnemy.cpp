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
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy() {
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AuraAbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AuraAbilitySystemComponent");
	AuraAbilitySystemComponent->SetIsReplicated(true);
	AuraAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 120, 0);
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

int32 AAuraEnemy::GetPlayerLevel() {
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

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	InitAbilityActorInfo();

	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AuraAbilitySystemComponent, CharacterClass);

	UAuraUserWidget* AuraUserWidget = CastChecked<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	AuraUserWidget->SetWidgetController(this);

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		OnHealthChanged.Broadcast(Data.NewValue);
	});

	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		OnMaxHealthChanged.Broadcast(Data.NewValue);
	});

	AuraAbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Action_HitReact, EGameplayTagEventType::NewOrRemoved).AddLambda([this](const FGameplayTag Tag, int32 NewCount) {
		bHitReacting = NewCount > 0;
		GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
		if (AuraAIController) {
			AuraAIController->GetBlackboardComponent()->SetValueAsBool("HitReacting", bHitReacting);
		}
	});

	OnHealthInitialize.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthInitialize.Broadcast(AuraAttributeSet->GetMaxHealth());
}

void AAuraEnemy::InitAbilityActorInfo() {
	Super::InitAbilityActorInfo();
	AuraAbilitySystemComponent->InitAbilityActorInfo(this, this);
	AuraAbilitySystemComponent->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AAuraEnemy::OnDie() {
	Super::OnDie();
	Dissolve();
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
