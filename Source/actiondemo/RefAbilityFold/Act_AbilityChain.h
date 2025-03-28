#pragma once
#include "CoreMinimal.h"
#include "Act_Ability.h"
#include "Act_AbilityDatas.h"
#include "Act_AbilityTypes.h"
#include "actiondemo/Act_TagContainer.h"
#include "Act_AbilityChain.generated.h"


UENUM(BlueprintType)
enum EAttackType
{
	RelaxAttack,
	HeavyAttack,
};
//ChainNode 相关内容制作
#pragma region ChainNode
UCLASS()
class ACTIONDEMO_API UAct_AbilityChainChildNode:public UObject
{
GENERATED_BODY()
public:
	//下一节的轻攻击和重攻击
	UPROPERTY()
	TObjectPtr<UAct_AbilityChainChildNode>  NextRelaxAttack=nullptr;
	UPROPERTY()
	TObjectPtr<UAct_AbilityChainChildNode>  NextHeavyAttack=nullptr;
	//本节所存储的技能：
	FAct_AbilityTypes SelfAbilityType;
	int32 length=0;
	//检查是否存在
	bool CheckNextIsValid(EAttackType CheckType) const 
	{
		switch (CheckType)
		{
		case RelaxAttack: return NextRelaxAttack!=nullptr; 
		case HeavyAttack: return NextHeavyAttack!=nullptr;
			
		}
		return false;
		
	}
	void initialNode(FAct_AbilityTypes AbilityTypes,int32 lengths)
	{
		
		SelfAbilityType=AbilityTypes;
		length=lengths;
	}
};
//作为技能树来设置
UCLASS()
class  ACTIONDEMO_API UAct_AbilityChainRoot:public UObject
{	GENERATED_BODY()
public:
	void Serlize(const UAct_AbilityDatasManager* datas)
	{	//获取数据中的技能
		delete PrimaryRelaxAbilityHead;
		PrimaryRelaxAbilityHead=nullptr;
		delete PrimaryHeavyAbilityHead;
		PrimaryHeavyAbilityHead=nullptr;
	    TArray<FAct_AbilityTypes> HeavyData=datas->AbilitTypesHeavyHead;
    	TArray<FAct_AbilityTypes> RelaxData=datas->AbilitTypesRelaxHead;
	    if (RelaxData.Num()>0)
	    {
		    PrimaryRelaxAbilityHead=NewObject<UAct_AbilityChainChildNode>();
	    	 PrimaryRelaxAbilityHead->initialNode(RelaxData[0],1);
	    }
	    if (HeavyData.Num()>0)
	    {
		    
	    	PrimaryHeavyAbilityHead=NewObject<UAct_AbilityChainChildNode>();
	    	PrimaryHeavyAbilityHead->initialNode(HeavyData[0],1);
	    }
		for (int i=1;i<RelaxData.Num();i++)
		{
			FAct_AbilityTypes CurrentAbilityType=RelaxData[i];
			FString CombatSequence=CurrentAbilityType.AbilityList;
			if (CombatSequence.Len()==0||CombatSequence[0]!='X') continue;
			UAct_AbilityChainChildNode* CurrentNode=PrimaryRelaxAbilityHead;
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
				CurrentNode=NextNode;
			}
		
		
		}
		
		for (int i=1;i<HeavyData.Num();i++)
		{
			FAct_AbilityTypes CurrentAbilityType=HeavyData[i];
			FString CombatSequence=CurrentAbilityType.AbilityList;
			if (CombatSequence.Len()==0||CombatSequence[0]!='Y') continue;
			UAct_AbilityChainChildNode* CurrentNode=PrimaryHeavyAbilityHead;
			for (int32 CharIndex=1;CharIndex<CombatSequence.Len();CharIndex++)
			{
				TCHAR CurrentChar=CombatSequence[CharIndex];
				EAttackType AttackType=(CurrentChar=='X')?EAttackType::RelaxAttack:EAttackType::HeavyAttack;
				TObjectPtr<UAct_AbilityChainChildNode>& NextNode=(AttackType==EAttackType::RelaxAttack)?CurrentNode->NextRelaxAttack:CurrentNode->NextHeavyAttack;
				if (!CurrentNode->CheckNextIsValid(AttackType))
				{
					NextNode=NewObject<UAct_AbilityChainChildNode>();
					NextNode->initialNode(CurrentAbilityType,CharIndex+1);
				}
				CurrentNode=NextNode;
			}
			
		}
		
	};
	UPROPERTY()
	TObjectPtr<UAct_AbilityChainChildNode> PrimaryRelaxAbilityHead=nullptr;
	UPROPERTY()
	TObjectPtr<UAct_AbilityChainChildNode> PrimaryHeavyAbilityHead=nullptr;
};
#pragma endregion  ChainNode
UCLASS()
class ACTIONDEMO_API UAct_AbilityChainManager : public UObject
{
	GENERATED_BODY()
public:
	//链表内容
	UPROPERTY()
	UAct_AbilityChainRoot * AbilityChainRoot=nullptr;
	UPROPERTY()
	TObjectPtr<UAct_AbilityDatas> AbilityData;
	UPROPERTY()
	UAct_AbilityChainChildNode * SelectedNode;
	//将分支传去下一个支
	//TODO::当不同情况下到某个位置时的技能，需要在AbilityType中放置一个技能数组，并且能够在序列化时向技能数组中放置并且能够排除一些枚举相同的技能比如当我都是在翻滚时按x 有两个翻滚时按x则只收录一个。
	UFUNCTION()
	virtual bool ToNextNode( UAct_AbilityChainChildNode * CurrentNode,EAttackType AttackType=EAttackType::RelaxAttack);
	UFUNCTION()
	virtual bool TurnToRoot();
	//游戏开始时调用在AbilitySystemComponent中调用
	UFUNCTION()
	virtual void BeginConstruct(const UAct_AbilityDatasManager* datas);
};
