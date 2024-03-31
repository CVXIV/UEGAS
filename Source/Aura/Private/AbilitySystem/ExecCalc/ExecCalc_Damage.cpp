// Copyright Cvxiv


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct FAuraDamageStatics {
	// 声明
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)

	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)

	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)

	FAuraDamageStatics() {
		// 定义
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
	}
};

static const FAuraDamageStatics& DamageStatics() {
	static FAuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage() {
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	const UAbilitySystemComponent* SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 获取基础伤害
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// 首先计算暴击之后的伤害
	float CriticalHitChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, CriticalHitChance);
	const float RealCriticalHitChance = UAuraAbilitySystemLibrary::Sigmoid_Modify(CriticalHitChance);
	float CriticalHitResistance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters, CriticalHitResistance);
	const float RealCriticalHitResistance = UAuraAbilitySystemLibrary::Sigmoid_Modify(CriticalHitResistance);
	const float FinalCriticalHitChance = RealCriticalHitChance - RealCriticalHitResistance;
	float CriticalHitDamage;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, CriticalHitDamage);
	Damage = UAuraAbilitySystemLibrary::ProbabilityCheck(FinalCriticalHitChance) ? 2 * Damage + CriticalHitDamage : Damage;

	float TargetArmor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	const float RealSourceArmorPenetration = UAuraAbilitySystemLibrary::Sigmoid_Modify(SourceArmorPenetration);
	const float EffectiveArmor = TargetArmor * (1.f - RealSourceArmorPenetration);
	if (EffectiveArmor >= 0) {
		Damage *= 1.f - EffectiveArmor / (100.f + EffectiveArmor);
	} else {
		Damage *= 1.f - EffectiveArmor / (100.f - EffectiveArmor);
	}

	float BlockChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, BlockChance);
	const float RealBlockChance = UAuraAbilitySystemLibrary::Sigmoid_Modify(BlockChance);
	Damage = UAuraAbilitySystemLibrary::ProbabilityCheck(RealBlockChance) ? Damage * 0.5f : Damage;

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
