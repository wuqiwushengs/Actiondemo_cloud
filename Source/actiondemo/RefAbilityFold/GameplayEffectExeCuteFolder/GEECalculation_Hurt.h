// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEECalculation_Hurt.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UGEECalculation_Hurt : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	UGEECalculation_Hurt();
	virtual  void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
