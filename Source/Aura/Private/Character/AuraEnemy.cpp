// Copyright Cvxiv
#include "Character/AuraEnemy.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
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

void AAuraEnemy::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	InitAbilityActorInfo();

	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AuraAbilitySystemComponent);

	UAuraUserWidget* AuraUserWidget = CastChecked<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	AuraUserWidget->SetWidgetController(this);

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		OnHealthChanged.Broadcast(Data.NewValue);
	});

	AuraAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) {
		OnMaxHealthChanged.Broadcast(Data.NewValue);
	});

	AuraAbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddLambda([this](const FGameplayTag Tag, int32 NewCount) {
		bHitReacting = NewCount > 0;
		GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
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
