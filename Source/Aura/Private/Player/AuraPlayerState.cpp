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
	DOREPLIFETIME(AAuraPlayerState, AttributePoints)
	DOREPLIFETIME(AAuraPlayerState, SpellPoints)
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const {
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddToXP(uint32 InXP) {
	const uint32 OldXP = this->XP;
	const uint32 NewXP = OldXP + InXP;
	const uint32 NewLevel = DataAssetLevelUpInfo->FindLevelForXp(NewXP);
	if (NewLevel != Level) {
		uint32 AttributePointReward = 0;
		uint32 SpellPointReward = 0;
		for (uint32 StartLevel = Level; StartLevel < NewLevel; ++StartLevel) {
			AttributePointReward += DataAssetLevelUpInfo->LevelUpInfo[StartLevel].AttributePointReward;
			SpellPointReward += DataAssetLevelUpInfo->LevelUpInfo[StartLevel].SpellPointReward;
		}
		AddToAttributePoints(AttributePointReward);
		AddToSpellPoints(SpellPointReward);

		AttributeSet->SetTopOffHealth(true);
		AttributeSet->SetTopOffMana(true);

		const uint32 OldLevel = Level;
		this->Level = NewLevel;
		OnLevelChangedDelegate.Broadcast(Level, OldLevel);
	}
	this->XP += InXP;

	OnXPChangedDelegate.Broadcast(this->XP, OldXP);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints) {
	const uint32 OldAttributePoints = AttributePoints;
	this->AttributePoints += InAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints, OldAttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints) {
	const uint32 OldSpellPoints = SpellPoints;
	this->SpellPoints += InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints, OldSpellPoints);
}

void AAuraPlayerState::OnRep_Level(uint32 OldLevel) const {
	OnLevelChangedDelegate.Broadcast(Level, OldLevel);
}

void AAuraPlayerState::OnRep_XP(uint32 OldXP) const {
	OnXPChangedDelegate.Broadcast(this->XP, OldXP);
}

void AAuraPlayerState::OnRep_AttributePoints(uint32 OldValue) const {
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints, OldValue);
}

void AAuraPlayerState::OnRep_SpellPoints(uint32 OldValue) const {
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints, OldValue);
}
