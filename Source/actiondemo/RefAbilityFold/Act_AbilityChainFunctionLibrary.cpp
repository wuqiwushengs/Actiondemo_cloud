// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilityChainFunctionLibrary.h"

#include "Act_AbilityTypes.h"
#include "actiondemo/Character/CharacterTypes.h"

bool UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(const TArray<FAct_AbilityTypes>& OwenTypes,FAct_AbilityTypes& CurrentType)
{
	for (FAct_AbilityTypes AbilityType : OwenTypes)
	{
		if (AbilityType.AttackingState==CurrentType.AttackingState)
		{
			return true;
		}
		continue;
	}
	
	return false;
}
