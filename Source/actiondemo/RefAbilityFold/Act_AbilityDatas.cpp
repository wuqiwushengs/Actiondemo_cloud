// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilityDatas.h"

#define RELAXATTACKNAME TEXT("X")
#define HEAVYATTACKNAME TEXT("Y")


FAct_AbilityTypes UAct_AbilityDatas::GetAbilityTypesNotInComboChainByTag(FGameplayTag InputTag)
{
	if(!AbilitiesNotInComboChain) return FAct_AbilityTypes();
	for(FName RowName:AbilitiesNotInComboChain->GetRowNames())
	{
		FString Msg=FString::Printf(TEXT("Can't find %s"),*RowName.ToString());
		FAct_AbilityTypes AbilityType=*AbilitiesNotInComboChain->FindRow<FAct_AbilityTypes>(RowName,Msg);
		if (AbilityType.InputTag==InputTag)
		{
			return AbilityType;
		}
	}
	return FAct_AbilityTypes();
}

void UAct_AbilityDatasManager::init()
{	ClearAbilityData();
	TMap<ECharacterUnAttackingState,FAct_Abilities> AbilityMap=LoadAbilityData();
	AbilitySum=LoadAbilitySum(AbilityMap);
}
void UAct_AbilityDatasManager::ClearAbilityData()
{	AbilitySum.AbilityTypesHeavyHead.Empty();
	AbilitySum.AbilityTypesRelaxHead.Empty();
}

TMap<ECharacterUnAttackingState,FAct_Abilities> UAct_AbilityDatasManager::LoadAbilityData()
{	TMap<ECharacterUnAttackingState,FAct_Abilities> FinalAbilityTypes;
	FAct_Abilities OneStateAbilities;
	//循环每个数据随后读取，读取完成之后添加到final后将OneStateAbilities置空
	for (TPair<ECharacterUnAttackingState,UDataTable*>& Data:AbilityData->AbilitiesContent)
	{
		if (!Data.Value) continue;
		ECharacterUnAttackingState CurrentState=Data.Key;
		TArray<FName> AbilitiesName=Data.Value->GetRowNames();
		for (int i=0;i<AbilitiesName.Num();i++)
		{
			FString Msg=FString::Printf(TEXT("Can't find %d content "),i);
			OneStateAbilities.AbilityTypes.Add(*Data.Value->FindRow<FAct_AbilityTypes>(AbilitiesName[i],Msg));
		}
		FinalAbilityTypes.Add(CurrentState,OneStateAbilities);
		OneStateAbilities.AbilityTypes.Empty();
	}
	return  FinalAbilityTypes;
}

FAbilitySum UAct_AbilityDatasManager::LoadAbilitySum(TMap<ECharacterUnAttackingState, FAct_Abilities> Ability)
{
	FAbilitySum FinalAbilitySum;
	for (TTuple<ECharacterUnAttackingState,FAct_Abilities> Data:Ability)
	{
		ECharacterUnAttackingState CurrentState=Data.Key;
		FAct_Abilities CurrentRelaxAbility=FAct_Abilities();
		FAct_Abilities CurrentHeavyAbility=FAct_Abilities();
		if (Data.Value.AbilityTypes.Num()<=0) continue;
		for (FAct_AbilityTypes & AbilityType :Data.Value.AbilityTypes)
		{
			TCHAR Value=AbilityType.GetAbilityListContentByIndex(0,false)[0];
			FString ValueStr;
			ValueStr.AppendChar(Value);
			if (ValueStr==RELAXATTACKNAME)
			{
				CurrentRelaxAbility.AbilityTypes.Add(AbilityType);
				
			
			}
			if (ValueStr==HEAVYATTACKNAME)
			{
				
				CurrentHeavyAbility.AbilityTypes.Add(AbilityType);
			}
		}
		CurrentRelaxAbility.AbilityTypes.Sort([](const FAct_AbilityTypes &A,const FAct_AbilityTypes &B)
	    {
			 if (A.AbilityList.Len()!=B.AbilityList.Len())
			 {
		       return A.AbilityList.Len()<B.AbilityList.Len();
			 }
			 return A.AbilityList.Right(1)==TEXT("X")&&B.AbilityList.Right(1)==TEXT("Y");
        });
		CurrentHeavyAbility.AbilityTypes.Sort([](const FAct_AbilityTypes &A,const FAct_AbilityTypes &B)
		{
			if (A.AbilityList.Len()!=B.AbilityList.Len())
			{
				return A.AbilityList.Len()<B.AbilityList.Len();
			}
			return A.AbilityList.Right(1)==TEXT("X")&&B.AbilityList.Right(1)==TEXT("Y");
		});
		FinalAbilitySum.AbilityTypesRelaxHead.Add(CurrentState,CurrentRelaxAbility);
		FinalAbilitySum.AbilityTypesHeavyHead.Add(CurrentState,CurrentHeavyAbility);
	}

	return FinalAbilitySum;
}


