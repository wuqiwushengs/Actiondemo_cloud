#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Act_AbilityChain.h"
#include "Act_AbilityDatas.h"
#include "actiondemo/InputFold/InputInfo.h"
#include "GameplayTagContainer.h"
#include "Act_AbilitySystemComponent.generated.h"
//用来标明该轻攻击和重攻击所用的字符，后续技能表中只允许添加这两个字符
#define RelaxAttackName "X"
#define HeavyAttackName "Y"
class UAct_AttributeSet;
struct FInputActionInstance;
//用来存储tag和世界时间
USTRUCT(BlueprintType)
struct FAbilityInputInfo
{
	GENERATED_BODY()
	FAbilityInputInfo(){};
	FAbilityInputInfo(FGameplayTag InTag): InputTag(InTag){};
	FAbilityInputInfo(FGameplayTag Intag,float InInputWordTime=0,float InInputIntervalTime=0,EInputWeightType InInputWeight=EInputWeightType::Weight0):InputTag(Intag),InputWordTime(InInputWordTime),InputIntervalTime(InInputIntervalTime),InputWeightType(InInputWeight){};
	UPROPERTY()
	FGameplayTag InputTag;
	UPROPERTY()
	float InputWordTime;
	//用来设置不同输入之间的时间间隔
	UPROPERTY()
	float InputIntervalTime;
	//其权重和InputData中的权重相同
	UPROPERTY()
	EInputWeightType InputWeightType;
};

UENUM(BlueprintType)
enum class InputState:uint8
{
	//普通输入节点
	NormalInputState,
	//禁止输入节点
	DisableInputState,
	//预输入节点
	PreInputState,
};
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FTimeLockDelegate,const FInputActionInstance&,InpuActiondata, UInputDataAsset *,InputDataAsset,FGameplayTag, InputTag);
DECLARE_DYNAMIC_DELEGATE(FTimerUnlockDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInputExecuteDelegate,const FAbilityInputInfo,Inputinfo);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONDEMO_API UAct_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UAct_AbilitySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	//Processing Function
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataStarted(const FInputActionInstance& ActionInstance, FGameplayTag Inputag,
	                                UInputDataAsset* InputDataAsset) ;
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataComplete(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset * InputDataAsset);
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataTrigger(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset * InputDataAsset,float TriggerTime);
	//InputLockFunction
	//仅仅用在输入时
	UFUNCTION()
	bool ChekcInputLengthToSetInputLock(float InputLength,const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag);
	UFUNCTION(BlueprintCallable)
	void SetInputLock(const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag);
	UFUNCTION(BlueprintCallable)
	void SetInputUnlock();
	//计算缓冲区存储的输入实际应该采用哪一个输入
	UFUNCTION(BlueprintCallable)
	bool  ExeAbilityInputInfo(const TArray<FAbilityInputInfo>& InputTagsBuff,FAbilityInputInfo& OutInputInfo);
	UFUNCTION()
	void CheckFinalInput();
	UFUNCTION(BlueprintCallable)
	void OnInputFinal(const FAbilityInputInfo& InputInfo);
#pragma region AbilityInputProcess
	//缓冲区记录输入的tag
	UPROPERTY(BlueprintReadOnly)
	TArray<FAbilityInputInfo> InputTagsInbuff;
	//单次输入的缓存最大时间
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	float AbilityInputBuffTime=0.1f;
	//通过设置此数据来对输入进行开锁或闭锁
	UPROPERTY(BlueprintReadOnly)
	InputState CurrentInputType;
	UPROPERTY()
	TMap<FGameplayTag,int> AllowedPreInput={{ActTagContainer::InputRelaxAttack,1},{ActTagContainer::InputHeavyAttack,1}
	,{ActTagContainer::InputDefense,1},{ActTagContainer::InputRolling,1}};
	void SetInputDisable(const FGameplayTagContainer  & DisableTag );
	void TurnPreInputToDefault();
	bool CheckIsAllowed(FGameplayTag PreInpuTag);
	UFUNCTION()
	void SetInputstate(InputState InputType);
	UPROPERTY()
	FTimerHandle FinalInputHandle;
	//用来处理在锁定之后的内容
	UPROPERTY()
	FTimeLockDelegate InputLockDelegate;
	UPROPERTY()
	FTimerUnlockDelegate InputUnlockDelegate;
	UFUNCTION()
	void OnPreSkillExecute(const FGameplayTag ExeTag, int32 count);
#pragma endregion AbiltyInputProcess
	//输入缓冲结束后执行
	UPROPERTY()
	FInputExecuteDelegate InputExecuteDelegate;
	//技能文件
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TObjectPtr<UAct_AbilityDatasManager> AbilityDataManager;
	//技能树文件
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TObjectPtr<UAct_AbilityChainManager> AbilityChainManager;
	
};
