// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "actiondemo/InputDataAsset.h"
#include "Act_AbilitySystemComponent.generated.h"
//用来标明该轻攻击和重攻击所用的字符，后续技能表中只允许添加这两个字符
#define RelaxAttackName "X"
#define HeavyAttackName "Y"
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FTimeLockDelegate,const FInputActionInstance&,InpuActiondata, UInputDataAsset *,InputDataAsset,FGameplayTag, InputTag);

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
	UPROPERTY()
	EInputWeightType InputWeightType;
	float CheckTimeInterval(const FAbilityInputInfo &FirstInputInfo) const;
};
inline float FAbilityInputInfo::CheckTimeInterval(const FAbilityInputInfo &FirstInputInfo) const
{
	float Intervaltime=this->InputWordTime-FirstInputInfo.InputWordTime;
	return Intervaltime;
}


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
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	//Processing Function
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataStarted(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset * InputDataAsset);
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataTriggering(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset * InputDataAsset);
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputDataComplete(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset * InputDataAsset);
	//InputLockFunction
	//仅仅用在输入时
	UFUNCTION()
	bool ChekcInputLengthToSetInputLock(float InputLength,const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag);
	UFUNCTION(BlueprintCallable)
	void SetInputLock(const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag);
	UFUNCTION(BlueprintCallable)
	void SetInputUnlock(const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag);

	//计算缓冲区存储的输入实际应该采用哪一个输入
	UFUNCTION(BlueprintCallable)
	bool  ExeAbilityInputInfo(const TArray<FAbilityInputInfo>& InputTagsBuff,FAbilityInputInfo& OutInputInfo);
	//缓冲区记录输入的tag
	UPROPERTY(BlueprintReadOnly)
	TArray<FAbilityInputInfo> InputTagsInbuff;
	bool InputLock;
	//通过设置此数据来对输入进行开锁或闭锁
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	float AbilityInputBuffTime=0.15;
	UFUNCTION()
	void CheckFinalInput();
	UPROPERTY()
	FTimerHandle FinalInputHandle;
	//用来处理在锁定之后的内容
	UPROPERTY()
	FTimeLockDelegate InputLockDelegate;
	UPROPERTY()
	FTimeLockDelegate InputUnlockDelegate;
};
