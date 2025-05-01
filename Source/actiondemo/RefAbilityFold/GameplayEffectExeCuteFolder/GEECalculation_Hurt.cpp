// Fill out your copyright notice in the Description page of Project Settings.


#include "GEECalculation_Hurt.h"
#include "actiondemo/RefAbilityFold/AttributeContent/Act_AttributeSet.h"

struct FDamageContentStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition StaminaDef;
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	FDamageContentStatics()
	{
		HealthDef=FGameplayEffectAttributeCaptureDefinition(UAct_AttributeSet::GetHealthAttribute(),EGameplayEffectAttributeCaptureSource::Target,true);
		StaminaDef=FGameplayEffectAttributeCaptureDefinition(UAct_AttributeSet::GetStaminaAttribute(),EGameplayEffectAttributeCaptureSource::Target,true);
		DamageDef=FGameplayEffectAttributeCaptureDefinition(UAct_AttributeSet::GetDamageAttribute(),EGameplayEffectAttributeCaptureSource::Source,true);
	}
};
static FDamageContentStatics & DamageStatic()
{
	static FDamageContentStatics statics;
	return statics;
}
UGEECalculation_Hurt::UGEECalculation_Hurt()
{
	RelevantAttributesToCapture.Add(DamageStatic().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatic().StaminaDef);
	RelevantAttributesToCapture.Add(DamageStatic().DamageDef);
}

void UGEECalculation_Hurt::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	const FGameplayEffectSpec & Spec=ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer * Source=Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer * Target=Spec.CapturedSourceTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameter;
	EvaluateParameter.SourceTags=Source;
	EvaluateParameter.TargetTags=Target;
	float Health =0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().HealthDef,EvaluateParameter,Health);
	float StaminaDef=0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().StaminaDef,EvaluateParameter,StaminaDef);
	float DamageDef=0.0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().DamageDef,EvaluateParameter,DamageDef);
	float MaxStaminaValue=100.0f;
	float DamageCost=1.2;
	//计算百分比减伤
	float  FirstDecrease=StaminaDef/(StaminaDef+MaxStaminaValue);
	float DamageAfterFirstDecrease=DamageDef*FirstDecrease;
	//伤害减少：
	const float NewDecreaseHealth=-1*(Health-FMath::Clamp(Health+DamageAfterFirstDecrease-DamageDef,0,Health));
	//削减韧性
	float DecreaseStamina=DamageDef*DamageCost;
	const float NewStamina=-1*(StaminaDef-FMath::Clamp(StaminaDef-DecreaseStamina,0,StaminaDef));
	TArray<FGameplayModifierEvaluatedData> Datas;
	Datas.Add(FGameplayModifierEvaluatedData(UAct_AttributeSet::GetHealthAttribute(),EGameplayModOp::Additive,NewDecreaseHealth));
	Datas.Add(FGameplayModifierEvaluatedData(UAct_AttributeSet::GetStaminaAttribute(),EGameplayModOp::Additive,NewStamina));
	Datas.Add(FGameplayModifierEvaluatedData(UAct_AttributeSet::GetDamageAttribute(),EGameplayModOp::Override,0));
	for (FGameplayModifierEvaluatedData &Data:Datas)
	{
		OutExecutionOutput.AddOutputModifier(Data);
	}
	
}
