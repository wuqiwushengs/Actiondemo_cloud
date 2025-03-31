// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Act_AbilityChainFunctionLibrary.generated.h"

struct FAct_AbilityTypes;
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_AbilityChainFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION()
	static bool CheckAbilityArrayHasContain(const TArray<FAct_AbilityTypes> & OwenTypes,FAct_AbilityTypes & CurrentType);
};
