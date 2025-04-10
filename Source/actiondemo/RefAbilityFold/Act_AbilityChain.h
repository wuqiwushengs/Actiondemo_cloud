#pragma once
#include "CoreMinimal.h"
#include "Act_Ability.h"
#include "Act_AbilityChainFunctionLibrary.h"
#include "Act_AbilityDatas.h"
#include "Act_AbilityTypes.h"
#include "actiondemo/Act_TagContainer.h"
#include "Act_AbilityChain.generated.h"
class UAct_AbilitySystemComponent;
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
	UPROPERTY(BlueprintReadOnly)
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
	//初始化并且对内部进行排序
	void initialNode(FAct_AbilityTypes &  AbilityTypes,int32 lengths,UAct_AbilitySystemComponent*AbilitySystemComponent);
//用来检查该节点正确的技能
	bool  CheckCorrectAbilityTypes(const UAct_AbilitySystemComponent * AbilitySystemComponent,FAct_AbilityTypes &AbilityTypes) const;
   bool OnGameplayChainIn(UAct_AbilitySystemComponent * AbilitySystemComponent,UAct_AbilityChainManager * ChainManager);
};
//作为技能树来设置
UCLASS()
class  ACTIONDEMO_API UAct_AbilityChainRoot:public UObject
{	GENERATED_BODY()
public:
	//初始化基础内容
	void Serlize(const TArray<FAct_AbilityTypes>& AbilitTypesRelaxHead,const TArray<FAct_AbilityTypes>&AbilitTypesHeavyHead,UAct_AbilitySystemComponent*AbilitySystemComponent);
	virtual void AddChainAbility(const TArray<FAct_AbilityTypes>& AbilityDatas,UAct_AbilityChainChildNode* ChooesdChainNode,UAct_AbilitySystemComponent * AbilitySystemComponent);
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
	UPROPERTY()
	TMap<FGameplayTag,FGameplayAbilitySpecHandle> UnComboHandle;
	//指向被选择分支的指针
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAct_AbilityChainChildNode> SelectedNode;
	UPROPERTY()
	FAct_AbilityTypes CurrentAbilityType;
	//后面在角色那里绑一个函数
	UPROPERTY()
	ECharacterUnAttackingState CurrentAbilityState=ECharacterUnAttackingState::Normal;
	UPROPERTY()
	ECharacterUnAttackingState LastAbilityState=ECharacterUnAttackingState::Normal;
	//获取AbilitSystemCompoent
	UPROPERTY()
	TObjectPtr<UAct_AbilitySystemComponent> AbilitySystemComponent;
	//将分支传去下一个支
	virtual bool ToNextNode(UAct_AbilityChainChildNode *&CurrentNode,EAttackType AttackType=EAttackType::RelaxAttack,ECharacterUnAttackingState CurrentState=ECharacterUnAttackingState::Normal);
	UFUNCTION()
	virtual bool TurnToRoot();
	//游戏开始时调用在AbilitySystemComponent中调用
	UFUNCTION()
	virtual void BeginConstruct(const UAct_AbilityDatasManager* datas,UAct_AbilitySystemComponent *Act_AbilitySystemComponent);

};
