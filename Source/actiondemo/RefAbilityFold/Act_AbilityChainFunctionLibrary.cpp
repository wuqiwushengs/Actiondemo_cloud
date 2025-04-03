// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilityChainFunctionLibrary.h"

#include "Act_AbilityTypes.h"
#include "actiondemo/Character/CharacterTypes.h"

bool UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(const TArray<FAct_AbilityTypes>& OwenTypes,FAct_AbilityTypes& CurrentType)
{
	if (OwenTypes.Num() == 0) return false;
	for (FAct_AbilityTypes AbilityType : OwenTypes)
	{
		if (CurrentType.OwnerRequiresTag.IsEmpty())
		{
			// 如果当前技能的标签为空，则只需检查已存在技能的标签是否也为空
			return AbilityType.OwnerRequiresTag.IsEmpty();
		}
		else
		{
			// 如果当前技能的标签不为空，则检查已存在技能是否包含其所有标签
			return AbilityType.OwnerRequiresTag.HasAll(CurrentType.OwnerRequiresTag);
		}

	}
	
	return false;
}
