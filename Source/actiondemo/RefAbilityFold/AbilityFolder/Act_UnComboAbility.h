// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "actiondemo/RefAbilityFold/Act_Ability.h"
#include "Act_UnComboAbility.generated.h"

/**
 * //因为没准备好基类，被迫继承写好的攻击技能类
 */
UCLASS()
class ACTIONDEMO_API UAct_UnComboAbility : public UAct_Ability
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UPROPERTY(EditDefaultsOnly)
	bool bTapped=true;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> EventOnReleased;
	UFUNCTION(BlueprintNativeEvent)
	void OnReleased(FGameplayEventData EventData);
	void OnReleased_Implementation( FGameplayEventData EventData);
};
