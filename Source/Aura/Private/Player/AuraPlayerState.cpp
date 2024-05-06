// Copyright Cvxiv


#include "Player/AuraPlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraPlayerAbilitySystemComponent.h"
#include "AbilitySystem/Data/AuraDataAssetLevelUpInfo.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState() {
	NetUpdateFrequency = 100;
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraPlayerAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level)
	DOREPLIFETIME(AAuraPlayerState, XP)
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const {
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddToXP(uint32 InXP) {
	const uint32 OldXP = this->XP;
	const uint32 NewXP = OldXP + InXP;
	const uint32 NewLevel = DataAssetLevelUpInfo->FindLevelForXP(NewXP);
	if (NewLevel != Level) {
		uint32 AttributePointReward = DataAssetLevelUpInfo->LevelUpInfo[NewLevel].AttributePointReward;
		uint32 SpellPointReward = DataAssetLevelUpInfo->LevelUpInfo[NewLevel].SpellPointReward;

		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
		AttributeSet->SetMana(AttributeSet->GetMaxMana());

		const uint32 OldLevel = Level;
		this->Level = NewLevel;
		OnLevelChangedDelegate.Broadcast(Level, OldLevel);
	}
	this->XP += InXP;

	OnXPChangedDelegate.Broadcast(this->XP, OldXP);
}

void AAuraPlayerState::OnRep_Level(uint32 OldLevel) const {
	OnLevelChangedDelegate.Broadcast(Level, OldLevel);
}

void AAuraPlayerState::OnRep_XP(uint32 OldXP) const {
	OnXPChangedDelegate.Broadcast(this->XP, OldXP);
}
