// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityInterFace/IAct_AbilityInterface.h"
#include "Act_Ability.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnpressedDelegate)
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_Ability : public UGameplayAbility,public IIAct_AbilityInterface
{
	GENERATED_BODY()
public:
	//如果是蓄力的话，则调用这个函数
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData*  TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	FOnpressedDelegate OnPressedDelegate;
	float AbilityTriggerTime;
	virtual void SetTriggerTime_Implementation(float TriggerTime) override;
	UFUNCTION()
	void OnAbilityTrigger(float AbilityTime=0);
};
