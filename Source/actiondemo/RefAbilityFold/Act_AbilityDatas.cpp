// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilityDatas.h"

#define RelaxAttackName TEXT("X")
#define HeavyAttackName TEXT("Y")



void UAct_AbilityDatasManager::init()
{	AbilitTypesRelaxHead.Empty();
	AbilitTypesHeavyHead.Empty();
	if (AbilityData->AbilitiesContent)
	{
		TArray<FName> AbilitiesName=AbilityData->AbilitiesContent->GetRowNames();
		TArray<FAct_AbilityTypes> AbilityTypes;
		for (int i=0;i<AbilitiesName.Num();i++)
		{
			FString Msg=FString::Printf(TEXT("Can't find %d content "),i);
			
			AbilityTypes.Add(*AbilityData->AbilitiesContent->FindRow<FAct_AbilityTypes>(AbilitiesName[i],Msg));
		}
		//排序获得的技能 并且根据轻攻击和重攻击分开
		for (FAct_AbilityTypes & AbilityType:AbilityTypes)
		{	
			TCHAR Value=AbilityType.GetAbilityListContentByIndex(0,false)[0];
			FString ValueStr;
			ValueStr.AppendChar(Value);
			if (ValueStr==RelaxAttackName)
			{
				AbilitTypesRelaxHead.Add(AbilityType);
				
			
			}
			if (ValueStr==HeavyAttackName)
			{
				
				AbilitTypesHeavyHead.Add(AbilityType);
			}
		}
	}
	AbilitTypesRelaxHead.Sort([](const FAct_AbilityTypes &A,const FAct_AbilityTypes &B)
	{
		if (A.AbilityList.Len()!=B.AbilityList.Len())
		{
			return A.AbilityList.Len()<B.AbilityList.Len();
		}
		return A.AbilityList.Right(1)==TEXT("X")&&B.AbilityList.Right(1)==TEXT("Y");
	});
	AbilitTypesHeavyHead.Sort([](const FAct_AbilityTypes &A,const FAct_AbilityTypes &B){
		if (A.AbilityList.Len()!=B.AbilityList.Len())
		{
			return A.AbilityList.Len()<B.AbilityList.Len();
		}
		return A.AbilityList.Right(1)==TEXT("X")&&B.AbilityList.Right(1)==TEXT("Y");
	});;
	
	//@TODO:将所有的技能根据长度前后进行排序，随后你现在已经获取了技能数据，后面的人物就是读取在AbilitysystemComponent中，并且完成技能的链表编写。
	
}
