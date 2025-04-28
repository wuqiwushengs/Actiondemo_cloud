// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "actiondemo/Character/Act_Character.h"
#include "actiondemo/Character/CharacterTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Act_Function.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_Function : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
   static 	EEigthDirectionState  CalculateMovementsDirection(float angle);
	UFUNCTION(BlueprintCallable)
	static  float GetOwnedWalkSpeed(AAct_Character * Character);
};
