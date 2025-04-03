#include "Act_AbilityChain.h"

#pragma region Act_AbilityChainRoot
void UAct_AbilityChainRoot::Serlize(const TArray<FAct_AbilityTypes>& AbilitTypesRelaxHead,const TArray<FAct_AbilityTypes>&AbilitTypesHeavyHead)
{
		//获取数据中的技能
		PrimaryRelaxAbilityHead=nullptr;
		PrimaryHeavyAbilityHead=nullptr;
		TArray<FAct_AbilityTypes> HeavyData=AbilitTypesHeavyHead;
		TArray<FAct_AbilityTypes> RelaxData=AbilitTypesRelaxHead;
		if (RelaxData.Num() > 0 && RelaxData[0].AbilityList.StartsWith("X"))
		{
			PrimaryRelaxAbilityHead=NewObject<UAct_AbilityChainChildNode>();
			for (FAct_AbilityTypes & Ability: RelaxData)
			{
				if (Ability.AbilityList.Len()==1&&!UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(PrimaryRelaxAbilityHead->SelfAbilityType,Ability))
				{
					PrimaryRelaxAbilityHead->initialNode(Ability,1);
				}
			}
			
		}
		if (HeavyData.Num() > 0 && HeavyData[0].AbilityList.StartsWith("Y"))
		{
			
			PrimaryHeavyAbilityHead=NewObject<UAct_AbilityChainChildNode>();
			for (FAct_AbilityTypes & Ability: HeavyData)
			{
				if (Ability.AbilityList.Len()==1&&!UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(PrimaryHeavyAbilityHead->SelfAbilityType,Ability))
				{
					PrimaryHeavyAbilityHead->initialNode(Ability,1);
				}
				break;
			}
			
		}
	
		AddChainAbility(RelaxData,PrimaryRelaxAbilityHead);
		AddChainAbility(HeavyData,PrimaryHeavyAbilityHead);	
		
		
	
}

void UAct_AbilityChainRoot::AddChainAbility(const TArray<FAct_AbilityTypes>& AbilityDatas,UAct_AbilityChainChildNode* ChooesdChainNode)
{
	for (int i=1;i<AbilityDatas.Num();i++)
	{
		FAct_AbilityTypes CurrentAbilityType=AbilityDatas[i];
		FString CombatSequence=CurrentAbilityType.AbilityList;
		if (CombatSequence.Len()==0) continue;
		UAct_AbilityChainChildNode* CurrentNode=ChooesdChainNode;
		for (int32 CharIndex = 1; CharIndex < CombatSequence.Len(); CharIndex++)
		{
			TCHAR CurrentChar = CombatSequence[CharIndex];
			// 判定攻击类型并获取对应的下一个节点
			EAttackType AttackType = (CurrentChar == 'X') ? EAttackType::RelaxAttack : EAttackType::HeavyAttack;
			TObjectPtr<UAct_AbilityChainChildNode>& NextNode = (AttackType == EAttackType::RelaxAttack) 
				? CurrentNode->NextRelaxAttack 
				: CurrentNode->NextHeavyAttack;

			// 如果下一个节点为空，则创建并初始化
			if (!NextNode)
			{
				NextNode = NewObject<UAct_AbilityChainChildNode>();
				NextNode->initialNode(CurrentAbilityType, CharIndex + 1);
			}
			// 确保当前能力类型不重复地添加到节点
			if (NextNode->length == CurrentAbilityType.AbilityList.Len() &&
				!UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(NextNode->SelfAbilityType, CurrentAbilityType))
			{
				NextNode->SelfAbilityType.Add(CurrentAbilityType);
			}

			// 更新当前节点为下一个节点
			CurrentNode = NextNode;
		}

		
	}
}
#pragma endregion Act_AbilityChainRoot
bool UAct_AbilityChainManager::ToNextNode(UAct_AbilityChainChildNode * CurrentNode,EAttackType AttackType,ECharacterUnAttackingState CurrentState)
{
	if (!CurrentNode || CurrentNode->SelfAbilityType[0].AttackingState != CurrentState)
	{
		TObjectPtr<UAct_AbilityChainChildNode>& TempNode = (AttackType == EAttackType::RelaxAttack) 
			? this->AbilityChainsRoot.FindChecked(CurrentState)->PrimaryRelaxAbilityHead 
			: this->AbilityChainsRoot.FindChecked(CurrentState)->PrimaryHeavyAbilityHead;

		CurrentNode = TempNode;
		return CurrentNode != nullptr;
	}

	TObjectPtr<UAct_AbilityChainChildNode>& TempNode = (AttackType == EAttackType::RelaxAttack) 
		? CurrentNode->NextRelaxAttack 
		: CurrentNode->NextHeavyAttack;

	if (TempNode)
	{
		CurrentNode = TempNode;
		return true;
	}

	return false;

	return false;
}

bool UAct_AbilityChainManager::TurnToRoot()
{
	this->SelectedNode=nullptr;
	if (!this->SelectedNode)
	{
		return true;
	}
	return false;
}
//TODO::目前技能树系统已经大改，需要你检查一下是否需要再某些地方进行安全检查避免缺少某一状态的技能而导致崩溃
void UAct_AbilityChainManager::BeginConstruct(const UAct_AbilityDatasManager* datas)
{	TArray<ECharacterUnAttackingState> CharacterUnAttackingStates;
	datas->AbilitySum.AbilityTypesRelaxHead.GetKeys(CharacterUnAttackingStates);
	if(CharacterUnAttackingStates.Num()<=0) return;
	for (ECharacterUnAttackingState State:CharacterUnAttackingStates)
	{
		UAct_AbilityChainRoot* Root=NewObject<UAct_AbilityChainRoot>();
		Root->Serlize(datas->AbilitySum.AbilityTypesRelaxHead.Find(State)->AbilityTypes,datas->AbilitySum.AbilityTypesHeavyHead.Find(State)->AbilityTypes);
		AbilityChainsRoot.Add(State,Root);
	}
	
}


