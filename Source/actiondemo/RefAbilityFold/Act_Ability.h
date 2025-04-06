// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Act_Ability.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnpressedDelegate)
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_Ability : public UGameplayAbility
{
	GENERATED_BODY()
public:
	//如果是蓄力的话，则调用这个函数
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData*  TriggerEventData) override;
	FOnpressedDelegate OnPressedDelegate;
};
