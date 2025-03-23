// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilityDatas.h"



UAct_AbilityDatas::UAct_AbilityDatas()
{
	if (AbilitiesContent)
	{
		TArray<FName> AbilitiesName=AbilitiesContent->GetRowNames();
		TArray<FAct_AbilityTypes> AbilityTypes;
		for (int i=0;i<AbilitiesName.Num();i++)
		{
			FString Msg=FString::Printf(TEXT("Can't find %d content "),i);
			
		    AbilityTypes.Add(*AbilitiesContent->FindRow<FAct_AbilityTypes>(AbilitiesName[i],Msg));
		}
		//排序获得的技能 并且根据轻攻击和重攻击分开
		for (FAct_AbilityTypes & AbilityType:AbilityTypes)
		{
			if (AbilityType.InputTag==ActTagContainer::RelaxAttack)
			{
				AbilitTypesRelaxHead.Add(AbilityType);
			}
			if (AbilityType.InputTag ==ActTagContainer::HeavyAttack)
			{
				AbilitTypesHeavyHead.Add(AbilityType);
			}
		}
	}
	//@TODO:将所有的技能根据长度前后进行排序，随后你现在已经获取了技能数据，后面的人物就是读取在AbilitysystemComponent中，并且完成技能的链表编写。
	
}

bool UAct_AbilityDatas::SortAbilityTypesByAttackType(const FAct_AbilityTypes &A,const FAct_AbilityTypes &B)
{
	return A.InputTag==ActTagContainer::RelaxAttack;
}
