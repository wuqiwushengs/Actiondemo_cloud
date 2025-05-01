// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "actiondemo/Act_TagContainer.h"
#include "actiondemo/Character/Act_Character.h"

UAct_AttributeSet::UAct_AttributeSet():Health(100.0f),MaxHealth(100.0f),Stamina(100.0f),MaxStamina(100.0f)
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
	//用来在受到攻击的时候播放受击动画
	if(Data.EvaluatedData.Attribute==GetHealthAttribute()&&Data.EvaluatedData.ModifierOp==EGameplayModOp::Additive)
	{  UE_LOG(LogTemp,Warning,TEXT("%f"),Data.EvaluatedData.Magnitude)
		FGameplayEventData EventData;
		EventData.Target=Data.Target.GetOwnerActor();
		EventData.Instigator=GetOwningActor();
		AAct_Character * Player=Cast<AAct_Character>(GetOwningActor());
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(),ActTagContainer::InterruptHurt,EventData);
	}
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
