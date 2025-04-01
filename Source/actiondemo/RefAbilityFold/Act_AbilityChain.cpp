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
			PrimaryRelaxAbilityHead->initialNode(RelaxData[0],1);
		}
		if (HeavyData.Num() > 0 && HeavyData[0].AbilityList.StartsWith("Y"))
		{
			
			PrimaryHeavyAbilityHead=NewObject<UAct_AbilityChainChildNode>();
			PrimaryHeavyAbilityHead->initialNode(HeavyData[0],1);
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
		for (int32 CharIndex=1;CharIndex<CombatSequence.Len();CharIndex++)
		{
			TCHAR CurrentChar=CombatSequence[CharIndex];
			EAttackType AttackType=(CurrentChar=='X')?EAttackType::RelaxAttack:EAttackType::HeavyAttack;
			TObjectPtr<UAct_AbilityChainChildNode>& NextNode=(AttackType==EAttackType::RelaxAttack)?CurrentNode->NextRelaxAttack:CurrentNode->NextHeavyAttack;
			if (!NextNode)
			{
				NextNode=NewObject<UAct_AbilityChainChildNode>();
				NextNode->initialNode(CurrentAbilityType,CharIndex+1);
			}
			if (NextNode->length==CurrentAbilityType.AbilityList.Len() && !UAct_AbilityChainFunctionLibrary::CheckAbilityArrayHasContain(NextNode->SelfAbilityType,CurrentAbilityType))
			{
				NextNode->SelfAbilityType.Add(CurrentAbilityType);
					
			}
			CurrentNode=NextNode;
		}
		
	}
}
#pragma endregion Act_AbilityChainRoot
bool UAct_AbilityChainManager::ToNextNode(UAct_AbilityChainChildNode * CurrentNode,EAttackType AttackType,ECharacterUnAttackingState CurrentState)
{
	if (CurrentNode==nullptr||CurrentNode->SelfAbilityType[0].AttackingState!=CurrentState)
	{   
		TObjectPtr<UAct_AbilityChainChildNode> &TempNode=(AttackType==EAttackType::RelaxAttack)?this->AbilityChainsRoot.FindChecked(CurrentState)->PrimaryRelaxAbilityHead:this->AbilityChainsRoot.FindChecked(CurrentState)->PrimaryHeavyAbilityHead;
		CurrentNode=TempNode;
		if (CurrentNode)
		{
			return true;
		}
	}
	if (TObjectPtr<UAct_AbilityChainChildNode> &TempNode=(AttackType==EAttackType::RelaxAttack)?CurrentNode->NextRelaxAttack:CurrentNode->NextHeavyAttack)
	{
		CurrentNode=TempNode;
		if (CurrentNode)
		{
			return true;
		}
	}
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

void UAct_AbilityChainManager::BeginConstruct(const UAct_AbilityDatasManager* datas)
{	TArray<ECharacterUnAttackingState> CharacterUnAttackingStates;
	datas->AbilityData->AbilitiesContent.GetKeys(CharacterUnAttackingStates);
	for (ECharacterUnAttackingState State:CharacterUnAttackingStates)
	{
		UAct_AbilityChainRoot* Root=NewObject<UAct_AbilityChainRoot>();
		Root->Serlize(datas->AbilitySum.AbilityTypesRelaxHead.Find(State)->AbilityTypes,datas->AbilitySum.AbilityTypesHeavyHead.Find(State)->AbilityTypes);
		AbilityChainsRoot.Add(State,Root);
	}
}

FAct_AbilityTypes UAct_AbilityChainManager::GetCorrectAbilityFromArray(UAct_AbilityChainChildNode* Chain,ECharacterUnAttackingState AttackingState)
{
	checkf(Chain,TEXT("Can't  Find Chain"));
	for (FAct_AbilityTypes& AbilityTypes:Chain->SelfAbilityType)
	{
		if (AbilityTypes.AttackingState==AttackingState)
		{
			return AbilityTypes;
		}
	}
	return FAct_AbilityTypes();
}
