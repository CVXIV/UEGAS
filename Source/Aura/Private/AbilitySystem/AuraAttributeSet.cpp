// Copyright Cvxiv


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet() {
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();

	TagsToAttribute.Add(AuraGameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);

	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);

	TagsToAttribute.Add(AuraGameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttribute.Add(AuraGameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// REPTNOTIFY_Always用于设置OnRep函数在客户端值已经与服务端同步的值相同的情况下触发(因为有预测)
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always)

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always)

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always)

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always)
}

// PreAttributeChange的参数NewValue就是这个不受约束的BaseValue，然后如果每次修改NewValue引用，其实最后就是修改CurrentValue，但本质的BaseValue没有修改
// 例如还有其他GE，它的modifier查询的结果仍然是未经Clamp的值（BaseValue），所以需要进行Set，例如SetHealth，此类方法其实修改的是BaseValue
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	} else if (Attribute == GetManaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) {
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute()) {
		if (bTopOffHealth) {
			SetHealth(GetMaxHealth());
			bTopOffHealth = false;
		} else {
			NewValue = FMath::Max(NewValue, 0);
			SetHealth(FMath::Min(NewValue, GetHealth()));
		}
	} else if (Attribute == GetMaxManaAttribute()) {
		if (bTopOffMana) {
			SetMana(GetMaxMana());
			bTopOffMana = false;
		} else {
			NewValue = FMath::Max(NewValue, 0);
			SetMana(FMath::Min(NewValue, GetMana()));
		}
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);

	if (Cast<ICombatInterface>(EffectProperties.TargetAvatarActor)) {
		if (ICombatInterface::Execute_IsDead(EffectProperties.TargetAvatarActor)) {
			EffectProperties.TargetAsc->RemoveActiveEffectsWithGrantedTags(Data.EffectSpec.Def->GetGrantedTags());
			return;
		}
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		const float NewHealth = GetHealth();
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		if (NewHealth <= 0) {
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor)) {
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(EffectProperties.EffectContextHandle));
			}
		}
	} else if (Data.EvaluatedData.Attribute == GetManaAttribute()) {
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	} else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute()) {
		HandleIncomingDamage(EffectProperties);
	} else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute()) {
		HandleIncomingXP(EffectProperties);
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth)
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(GetOwningAbilitySystemComponentChecked()->GetAvatarActor())) {
		EnemyInterface->SetHealthReplicated();
	}
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth)
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(GetOwningAbilitySystemComponentChecked()->GetAvatarActor())) {
		EnemyInterface->SetMaxHealthReplicated();
	}
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana)
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana)
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength)
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence)
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience)
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor)
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor)
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration)
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance)
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance)
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage)
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance)
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration)
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration)
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldAttributeData) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldAttributeData)
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldAttributeData) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldAttributeData)
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldAttributeData) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldAttributeData)
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldAttributeData) const {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldAttributeData)
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) {
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceAsc = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(Props.SourceAsc) && Props.SourceAsc->AbilityActorInfo.IsValid() && Props.SourceAsc->AbilityActorInfo->AvatarActor.IsValid()) {
		Props.SourceAvatarActor = Props.SourceAsc->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceAsc->AbilityActorInfo->PlayerController.Get();
		if (!Props.SourceController && Props.SourceAvatarActor) {
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor)) {
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController) {
			Props.SourceCharacter = Props.SourceController->GetCharacter();
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid()) {
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetAsc = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props) {
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetCharacter);
	const ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
	const int32 Level = CombatInterface->GetPlayerLevel();
	const float XPReward = UAuraAbilitySystemLibrary::GetXPForCharacterClassAndLevel(Props.TargetCharacter, CharacterClass, Level);

	FGameplayEventData Payload;
	Payload.EventTag = FAuraGameplayTags::Get().Attributes_Meta_IncomingXP;
	Payload.EventMagnitude = XPReward;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, Payload.EventTag, Payload);
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props) {
	const float LocalInComingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(Props.SourceCharacter)) {
		PlayerInterface->AddToXP(LocalInComingXP);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props) {
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage >= 0) {
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		if (NewHealth <= 0.f) {
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor)) {
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));

				SendXPEvent(Props);
			}
		} else {
			if (UAuraAbilitySystemLibrary::IsTakeHitReact(Props.EffectContextHandle)) {
				FGameplayTagContainer TagContainerCancel;
				TagContainerCancel.AddTag(FAuraGameplayTags::Get().Action_HitReact);
				Props.TargetAsc->CancelAbilities(&TagContainerCancel);
				Props.TargetAsc->TryActivateAbilitiesByTag(TagContainerCancel);
			}

			const FVector& KnockBackForce = UAuraAbilitySystemLibrary::GetKnockBackForce(Props.EffectContextHandle);
			if (!KnockBackForce.IsZero()) {
				Props.TargetCharacter->LaunchCharacter(KnockBackForce, false, false);
			}
		}
		const bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		const bool bIsBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Props.SourceController);
		if (!PlayerController) {
			PlayerController = Cast<AAuraPlayerController>(Props.TargetController);
		}
		if (PlayerController) {
			PlayerController->ClientShowWidget(Props.TargetAvatarActor, LocalIncomingDamage, bIsCriticalHit, bIsBlockedHit);
		}

		// todo 这里SourceAsc可能已经不存在（被杀死）
		FGameplayEffectContextHandle EffectContextHandle = Props.SourceAsc->MakeEffectContext();
		EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);

		const TArray<FDeBuffProperty>& DeBuffProperties = UAuraAbilitySystemLibrary::GetDeBuffProperty(Props.EffectContextHandle);
		for (const FDeBuffProperty& DeBuffProperty : DeBuffProperties) {
			if (DeBuffProperty.bIsSuccessfulDeBuff) {
				FString DeBuffName = FString::Printf(TEXT("DynamicDeBuff_%s"), *DeBuffProperty.DeBuff.ToString());
				UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DeBuffName));

				GameplayEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
				GameplayEffect->Period = DeBuffProperty.DeBuffFrequency;
				GameplayEffect->DurationMagnitude = FScalableFloat(DeBuffProperty.DeBuffDuration);
				GameplayEffect->bExecutePeriodicEffectOnApplication = false;

				UTargetTagsGameplayEffectComponent& TargetTags = GameplayEffect->AddComponent<UTargetTagsGameplayEffectComponent>();
				FInheritedTagContainer TagContainerMods;
				TagContainerMods.AddTag(DeBuffProperty.DeBuff);
				TargetTags.SetAndApplyTargetTagChanges(TagContainerMods);

				GameplayEffect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
				GameplayEffect->StackLimitCount = 1;

				if (DeBuffProperty.DeBuffDamage > 0) {
					const int32 Index = GameplayEffect->Modifiers.Num();
					GameplayEffect->Modifiers.Add(FGameplayModifierInfo());
					FGameplayModifierInfo& ModifierInfo = GameplayEffect->Modifiers[Index];
					ModifierInfo.ModifierMagnitude = FScalableFloat(DeBuffProperty.DeBuffDamage);
					ModifierInfo.ModifierOp = EGameplayModOp::Additive;
					ModifierInfo.Attribute = GetIncomingDamageAttribute();
				}

				if (const FGameplayEffectSpec* EffectSpec = new FGameplayEffectSpec(GameplayEffect, EffectContextHandle, 1.f)) {
					//FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
					//AuraContext->AddDeBuffInfo()
					Props.TargetAsc->ApplyGameplayEffectSpecToSelf(*EffectSpec);
				}
			}
		}
	}
}
