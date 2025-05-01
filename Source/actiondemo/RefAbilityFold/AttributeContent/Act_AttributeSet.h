// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Act_AttributeSet.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
UCLASS()
class ACTIONDEMO_API UAct_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAct_AttributeSet();
	UPROPERTY()
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAct_AttributeSet,Health);
	UPROPERTY()
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAct_AttributeSet,MaxHealth);
	UPROPERTY()
	FGameplayAttributeData  Stamina;
	ATTRIBUTE_ACCESSORS(UAct_AttributeSet,Stamina);
	UPROPERTY()
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UAct_AttributeSet,MaxStamina);
	UPROPERTY()
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAct_AttributeSet,Damage);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)  override;
	UFUNCTION()
	void OnMaxAttribuetChange(FGameplayAttributeData & AffectedAttribute,const FGameplayAttributeData & MaxAttributeData,float & NewValue,const FGameplayAttribute & AfterAffectedAttribute);
};
