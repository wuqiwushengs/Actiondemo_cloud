// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_UnComboAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "actiondemo/Act_TagContainer.h"


void UAct_UnComboAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{   
	UGameplayAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//绑定按键释放的逻辑
	if (!bTapped)
	{
		EventOnReleased = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ActTagContainer::ExeUnComboAbilityInputReleased);
		EventOnReleased->EventReceived.AddDynamic(this, &UAct_UnComboAbility::OnReleased);
		EventOnReleased->Activate();
	}
}

void UAct_UnComboAbility::OnReleased_Implementation( FGameplayEventData EventData)
{
	
}
