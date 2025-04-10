// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityInterFace/IAct_AbilityInterface.h"
#include "Act_AbilityBase.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnpressedDelegate)
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_AbilityBase : public UGameplayAbility,public IIAct_AbilityInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void SetTriggerTime_Implementation(float TriggerTime) override;
	UFUNCTION()
	virtual void  OnAbilityTrigger(float AbilityTime=0);
};
