// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AttributeSet.h"

UAct_AttributeSet::UAct_AttributeSet():Health(100.0f),MaxHealth(100.0f),Stamina(1.0f),MaxStamina(1.0f)
{
}

void UAct_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute==GetHealthAttribute())
	{
        NewValue=FMath::Clamp(NewValue,0,GetMaxHealth());		
	}
	if (Attribute==GetMaxHealthAttribute())
	{
		OnMaxAttribuetChange(Health,MaxHealth,NewValue,GetHealthAttribute());
	}
	if (Attribute==GetStaminaAttribute())
	{
		NewValue=FMath::Clamp(NewValue,0,GetMaxStamina());
	}
	if (Attribute==GetMaxStaminaAttribute())
	{
		OnMaxAttribuetChange(Stamina,MaxStamina,NewValue,GetStaminaAttribute());
	}
}

void UAct_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}

void UAct_AttributeSet::OnMaxAttribuetChange(FGameplayAttributeData& AffectedAttribute,
                                             const FGameplayAttributeData& MaxAttributeData, float& NewValue, const FGameplayAttribute& AfterAffectedAttribute)
{
	UAbilitySystemComponent *Asc=GetOwningAbilitySystemComponent();
	if (!FMath::IsNearlyEqual(MaxAttributeData.GetCurrentValue(),NewValue))
	{
		float Delta=(MaxAttributeData.GetCurrentValue()>0.0f)?NewValue/MaxAttributeData.GetCurrentValue()*AffectedAttribute.GetCurrentValue():NewValue;
		Asc->ApplyModToAttribute(AfterAffectedAttribute,EGameplayModOp::Additive,Delta);
	}
	
}
