#include "Act_AbilityChain.h"

bool UAct_AbilityChainManager::ToNextNode(UAct_AbilityChainChildNode* CurrentNode,EAttackType AttackType)
{
	if (CurrentNode==nullptr)
	{
		TObjectPtr<UAct_AbilityChainChildNode> &TempNode=(AttackType==EAttackType::RelaxAttack)?AbilityChainRoot->PrimaryRelaxAbilityHead:AbilityChainRoot->PrimaryHeavyAbilityHead;
		CurrentNode=TempNode;
		if (CurrentNode)
		{
			return true;
		}
		return false;
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
{
	AbilityChainRoot=NewObject<UAct_AbilityChainRoot>();
	AbilityChainRoot->Serlize(datas);
}
