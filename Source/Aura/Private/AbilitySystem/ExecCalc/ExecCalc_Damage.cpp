// Copyright Cvxiv


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/GameplayStatics.h"

struct FAuraDamageStatics {
	friend class UExecCalc_Damage;

	// 声明
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)

	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)

	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)

	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)

	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetTagsToCaptureDefs() const {
		return TagsToCaptureDefs;
	}

	FAuraDamageStatics() {
		// 定义
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false)
	}

private:
	void InitializeTagsToCaptureDefs() {
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
};

static FAuraDamageStatics& DamageStatics() {
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
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);

	FAuraGameplayTags::S_OnGameplayTagsInitialized.AddLambda([]() {
		DamageStatics().InitializeTagsToCaptureDefs();
	});
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

	FGameplayEffectContextHandle EffectSpecHandle = Spec.GetContext();
	FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectSpecHandle.Get());
	// 获取基础伤害和DeBuff
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, float>& Pair : Spec.SetByCallerTagMagnitudes) {
		if (const FDeBuffInfo* DeBuffInfo = FAuraGameplayTags::Get().DamageTypesToDeBuffAndResistance.Find(Pair.Key)) {
			const FGameplayEffectAttributeCaptureDefinition& CaptureDefinition = DamageStatics().GetTagsToCaptureDefs()[DeBuffInfo->ResistanceTag];
			// 抗性可以减少此类伤害，也可以降低此类伤害对应的DeBuff产生概率
			float ResistanceValue;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDefinition, EvaluateParameters, ResistanceValue);
			const float RealResistanceValue = UAuraAbilitySystemLibrary::Sigmoid_Modify(ResistanceValue);
			Damage += Pair.Value * (1.f - RealResistanceValue);

			const float DeBuffChance = UAuraAbilitySystemLibrary::Sigmoid_Modify(Spec.GetSetByCallerMagnitude(DeBuffInfo->DeBuff_Chance, false));
			const float FinalDeBuffChance = DeBuffChance - UAuraAbilitySystemLibrary::Sigmoid_Modify(RealResistanceValue);
			if (const bool bDeBuff = UAuraAbilitySystemLibrary::ProbabilityCheck(FinalDeBuffChance)) {
				FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

				const float DeBuffDamage = Spec.GetSetByCallerMagnitude(DeBuffInfo->DeBuff_Damage, false);
				const float DeBuffFrequency = Spec.GetSetByCallerMagnitude(DeBuffInfo->DeBuff_Frequency, false);
				const float DeBuffDuration = Spec.GetSetByCallerMagnitude(DeBuffInfo->DeBuff_Duration, false);
				UAuraAbilitySystemLibrary::SetDeBuffProperty(EffectContextHandle, bDeBuff, DeBuffInfo->DeBuffTag, DeBuffDamage, DeBuffFrequency, DeBuffDuration);
			}
		}
	}

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
	const bool bIsCriticalHit = UAuraAbilitySystemLibrary::ProbabilityCheck(FinalCriticalHitChance);
	Damage = bIsCriticalHit ? 2 * Damage + CriticalHitDamage : Damage;
	EffectContext->SetIsCriticalHit(bIsCriticalHit);

	// 接下来如果是径向伤害，先计算衰减后的值
	if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectSpecHandle)) {
		const FVector RadialDamageOrigin = UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectSpecHandle);
		const float DamageInnerRadius = UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectSpecHandle);
		const float DamageOuterRadius = UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectSpecHandle);
		Damage = UAuraAbilitySystemLibrary::CalcRadialDamageWithFalloff(Damage, FVector::Distance(TargetAvatar->GetActorLocation(), RadialDamageOrigin), DamageInnerRadius, DamageOuterRadius, 0.5f);
	}

	float TargetArmor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);

	float SourceArmorPenetration;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	const float EffectiveArmor = TargetArmor - SourceArmorPenetration;
	if (EffectiveArmor >= 0) {
		Damage *= 1.f - EffectiveArmor / (100.f + EffectiveArmor);
	} else {
		Damage *= 1.f - EffectiveArmor / (100.f - EffectiveArmor);
	}

	float BlockChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, BlockChance);
	const float RealBlockChance = UAuraAbilitySystemLibrary::Sigmoid_Modify(BlockChance);
	const bool bIsBlock = UAuraAbilitySystemLibrary::ProbabilityCheck(RealBlockChance);
	Damage = bIsBlock ? Damage * 0.5f : Damage;
	EffectContext->SetIsBlockedHit(bIsBlock);

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
