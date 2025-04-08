// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Ability.h"


void UAct_Ability::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	OnPressedDelegate.AddLambda([this]()
	{
		EndAbility(this->GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
	});
}

void UAct_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo)) EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	//技能前摇
	//蓄力技能中
	//技能后摇
	
}

void UAct_Ability::SetTriggerTime_Implementation(float AbilityTime)
{
	this->AbilityTriggerTime=AbilityTime;
	OnAbilityTrigger(AbilityTriggerTime);
	
}

void UAct_Ability::OnAbilityTrigger(float TriggerTime)
{
	//处理所有持续时间的技能的一些效果。
	
	
}
