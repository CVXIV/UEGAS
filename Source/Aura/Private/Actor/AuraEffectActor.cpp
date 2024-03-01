// Copyright Cvxiv


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor() {
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay() {
	Super::BeginPlay();
}

bool AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect> GameplayEffectClass) const {
	UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!Asc) {
		return false;
	}

	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = Asc->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = Asc->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	return true;
}