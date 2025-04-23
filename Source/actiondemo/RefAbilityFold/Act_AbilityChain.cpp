#include "Act_AbilityChain.h"
#include "Act_AbilitySystemComponent.h"
#include "actiondemo/Character/CharacterInferface.h"
#pragma region Act_AbilityChainRoot
void UAct_AbilityChainChildNode::initialNode(FAct_AbilityTypes& AbilityTypes, int32 lengths,UAct_AbilitySystemComponent* AbilitySystemComponent)
{
	//初始化时添加内部技能并且向AbilitySystemCompoent组件赋予技能，并且将其委托绑定到types的handle中
	length=lengths;
	FGameplayAbilitySpec AbilitySpec(AbilityTypes.Ability);
	AbilityTypes.Handle=AbilitySystemComponent->GiveAbility(AbilitySpec);
	check(AbilityTypes.Handle.IsValid());
	SelfAbilityType.Add(AbilityTypes);
	//根据内部是否为空排序
	SelfAbilityType.Sort([](const FAct_AbilityTypes & A,const FAct_AbilityTypes& B)
	{
		return !A.InputTag.IsValid()&&B.InputTag.IsValid();
	});
}
void UAct_AbilityChainRoot::Serlize(const TArray<FAct_AbilityTypes>& AbilitTypesRelaxHead,const TArray<FAct_AbilityTypes>&AbilitTypesHeavyHead,UAct_AbilitySystemComponent*AbilitySystemComponent)
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
					PrimaryRelaxAbilityHead->initialNode(Ability,1,AbilitySystemComponent);
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
					PrimaryHeavyAbilityHead->initialNode(Ability,1,AbilitySystemComponent);
				}
				break;
			}
			
		}
	
		AddChainAbility(RelaxData,PrimaryRelaxAbilityHead,AbilitySystemComponent);
		AddChainAbility(HeavyData,PrimaryHeavyAbilityHead,AbilitySystemComponent);	
		
		
	
}
void UAct_AbilityChainRoot::AddChainAbility(const TArray<FAct_AbilityTypes>& AbilityDatas,UAct_AbilityChainChildNode* ChooesdChainNode,UAct_AbilitySystemComponent * AbilitySystemComponent)
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
				NextNode->initialNode(CurrentAbilityType, CharIndex + 1,AbilitySystemComponent);
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
//检查是否有正确的技能类
bool UAct_AbilityChainChildNode::CheckCorrectAbilityTypes(const UAct_AbilitySystemComponent* AbilitySystemComponent,FAct_AbilityTypes& AbilityTypes) const
{	FGameplayTagContainer AbilityTags=AbilitySystemComponent->GetOwnedGameplayTags();
	for (FAct_AbilityTypes AbilityType :SelfAbilityType)
	{
		if (AbilityTags.HasAllExact(AbilityType.OwnerRequiresTag))
		{
			AbilityTypes=AbilityType;
			return true;
		}
			
	};
	UE_LOG(LogTemp,Warning,TEXT("do once"))
	return false;
}
//当进入这个技能组件的时候尝试执行技能。
bool UAct_AbilityChainChildNode::OnGameplayChainIn(UAct_AbilitySystemComponent * AbilitySystemComponent,UAct_AbilityChainManager * ChainManager)
{	FAct_AbilityTypes EnableAbilityTypes;
	if (CheckCorrectAbilityTypes(AbilitySystemComponent,EnableAbilityTypes))
	{
		bool Success=AbilitySystemComponent->TryActivateAbilityByClass(EnableAbilityTypes.Ability);
		ChainManager->CurrentAbilityType=EnableAbilityTypes;
		return Success;
	}
	return false;
}
#pragma endregion Act_AbilityChainRoot
bool UAct_AbilityChainManager::ToNextNode(UAct_AbilityChainChildNode *& CurrentNode,EAttackType AttackType,ECharacterUnAttackingState CurrentState)
{	//如果当前阶段的状态和当前的状态不一样那么久从新赋值
	
	if (LastAbilityState!= CurrentState||!SelectedNode&&AbilityChainsRoot[CurrentState])
	{	
		 CurrentNode= (AttackType == EAttackType::RelaxAttack) 
			? this->AbilityChainsRoot[CurrentState]->PrimaryRelaxAbilityHead 
			: this->AbilityChainsRoot[CurrentState]->PrimaryHeavyAbilityHead;
		if (!CurrentNode)return false;
		CurrentNode->OnGameplayChainIn(AbilitySystemComponent,this);
		LastAbilityState=CurrentState;
		SelectedNode=CurrentNode;
		return true;
	}
	if (CurrentNode)
	{
		//如果一样那么向检查，如过检查不到那么
		CurrentNode= (AttackType == EAttackType::RelaxAttack) 
			? CurrentNode->NextRelaxAttack 
			: CurrentNode->NextHeavyAttack;
	}
	//如果没有下一个阶段则直接不执行任何内容了
	if (!CurrentNode)
	{
		TurnToRoot();
		return false;
	}
	CurrentNode->OnGameplayChainIn(AbilitySystemComponent,this);
	LastAbilityState=CurrentState;
	SelectedNode=CurrentNode;
	return true;
	
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
void UAct_AbilityChainManager::BeginConstruct(const UAct_AbilityDatasManager* datas,UAct_AbilitySystemComponent *Act_AbilitySystemComponent)
{	
	this->AbilitySystemComponent=Act_AbilitySystemComponent;
	TArray<ECharacterUnAttackingState> CharacterUnAttackingStates;
	datas->AbilitySum.AbilityTypesRelaxHead.GetKeys(CharacterUnAttackingStates);
	SelectedNode=nullptr;
	CurrentAbilityType=FAct_AbilityTypes();
	if(CharacterUnAttackingStates.Num()<=0) return;
	for (ECharacterUnAttackingState State:CharacterUnAttackingStates)
	{
		UAct_AbilityChainRoot* Root=NewObject<UAct_AbilityChainRoot>();
		Root->Serlize(datas->AbilitySum.AbilityTypesRelaxHead.Find(State)->AbilityTypes,datas->AbilitySum.AbilityTypesHeavyHead.Find(State)->AbilityTypes,AbilitySystemComponent);
		AbilityChainsRoot.Add(State,Root);
	}
	if (!datas->AbilityData->AbilitiesNotInComboChain) return;
	for(FName Name:datas->AbilityData->AbilitiesNotInComboChain->GetRowNames())
	{	FString MSG=TEXT("Can't find");
		FGameplayAbilitySpec Spec(datas->AbilityData->AbilitiesNotInComboChain->FindRow<FAct_AbilityTypes>(Name,MSG)->Ability);
		FGameplayTag Tag=datas->AbilityData->AbilitiesNotInComboChain->FindRow<FAct_AbilityTypes>(Name,MSG)->InputTag;
		FGameplayAbilitySpecHandle Handle=Act_AbilitySystemComponent->GiveAbility(Spec);
		UnComboHandle.Add(Tag,Handle);
	};
}


