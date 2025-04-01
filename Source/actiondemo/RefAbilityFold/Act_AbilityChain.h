#pragma once
#include "CoreMinimal.h"
#include "Act_Ability.h"
#include "Act_AbilityChainFunctionLibrary.h"
#include "Act_AbilityDatas.h"
#include "Act_AbilityTypes.h"
#include "actiondemo/Act_TagContainer.h"
#include "Act_AbilityChain.generated.h"


class UAct_AbilityChainFunctionLibrary;

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
	TArray<FAct_AbilityTypes> SelfAbilityType;
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
	void initialNode(const FAct_AbilityTypes &  AbilityTypes,int32 lengths)
	{
		
		SelfAbilityType.Add(AbilityTypes);
		length=lengths;
	}
};
//作为技能树来设置
UCLASS()
class  ACTIONDEMO_API UAct_AbilityChainRoot:public UObject
{	GENERATED_BODY()
public:
	//初始化基础内容
	void Serlize(const TArray<FAct_AbilityTypes>& AbilitTypesRelaxHead,const TArray<FAct_AbilityTypes>&AbilitTypesHeavyHead);
	
	virtual void AddChainAbility(const TArray<FAct_AbilityTypes>& AbilityDatas,UAct_AbilityChainChildNode* ChooesdChainNode);
public:
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
	TMap<ECharacterUnAttackingState,UAct_AbilityChainRoot*> AbilityChainsRoot;
	//指向被选择分支的指针
	UPROPERTY()
	UAct_AbilityChainChildNode * SelectedNode;
	//后面在角色那里绑一个函数
	UPROPERTY()
	ECharacterUnAttackingState CurrentAbilityState;
	//将分支传去下一个支
	//TODO::当不同情况下到某个位置时的技能，需要在AbilityType中放置一个技能数组，并且能够在序列化时向技能数组中放置并且能够排除一些枚举相同的技能比如当我都是在翻滚时按x 有两个翻滚时按x则只收录一个。
	UFUNCTION()
	virtual bool ToNextNode(UAct_AbilityChainChildNode * CurrentNode,EAttackType AttackType=EAttackType::RelaxAttack,ECharacterUnAttackingState CurrentState=ECharacterUnAttackingState::Normal);
	UFUNCTION()
	virtual bool TurnToRoot();
	//游戏开始时调用在AbilitySystemComponent中调用
	UFUNCTION()
	virtual void BeginConstruct(const UAct_AbilityDatasManager* datas);
	UFUNCTION()
	virtual FAct_AbilityTypes GetCorrectAbilityFromArray(UAct_AbilityChainChildNode * Chain,ECharacterUnAttackingState AttackingState=ECharacterUnAttackingState::Normal);

};
