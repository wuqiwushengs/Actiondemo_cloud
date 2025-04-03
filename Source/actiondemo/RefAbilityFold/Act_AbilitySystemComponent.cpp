// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilitySystemComponent.h"



// Sets default values for this component's properties
UAct_AbilitySystemComponent::UAct_AbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InputLock=false;
	//初始化
	AbilityDataManager=CreateDefaultSubobject<UAct_AbilityDatasManager>("AbilityData");
	AbilityChainManager=CreateDefaultSubobject<UAct_AbilityChainManager>("AbilityChainManager");
	// ...
}
// Called when the game starts
void UAct_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	//初始化数据：
	if (AbilityDataManager)
	{
		AbilityDataManager->init();
	}
	if (AbilityChainManager)
	{
		AbilityChainManager->BeginConstruct(AbilityDataManager);
	}
	InputExecuteDelegate.BindUFunction(this,TEXT("OnInputFinal"));
	// ...
	
}
void UAct_AbilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UAct_AbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}
void UAct_AbilitySystemComponent::ProcessingInputDataStarted(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{  //获得当前所有输入的tag以及输入的世界时间
	//处理输入缓存设置
	float WordTime=ActionInstance.GetLastTriggeredWorldTime();
	//TODO::设置线程锁判断，预输入开启线程的动画通知，变为normal状态时开启线程。
	FAbilityInputInfo Abilityinfo(Inputag,WordTime,InputTagsInbuff.Num()<=0?0:WordTime-InputTagsInbuff[0].InputWordTime,InputDataAsset->GetAbilityInputDatabyTag(Inputag).InputType);
	if (ChekcInputLengthToSetInputLock(Abilityinfo.InputIntervalTime,ActionInstance,InputDataAsset,Inputag))
	{
		InputTagsInbuff.Add(Abilityinfo);
	}
	if (InputTagsInbuff.Num()==1&&!GetWorld()->GetTimerManager().IsTimerActive(FinalInputHandle))
	{
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,FString::Printf(TEXT("%f"),GetWorld()->GetTime().GetRealTimeSeconds()));
		//假如已经绑定了那就不再进行绑定
		int32 index=this->InputTagsInbuff.Num()-1;
		FTimerDelegate FinalExecute;
		FinalExecute.BindLambda([this,ActionInstance,InputDataAsset,index](){CheckFinalInput();SetInputLock(ActionInstance,InputDataAsset,this->InputTagsInbuff[index].InputTag);});
		GetWorld()->GetTimerManager().SetTimer(FinalInputHandle,FinalExecute,AbilityInputBuffTime,false);
	}

}
void UAct_AbilitySystemComponent::ProcessingInputDataTriggering(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{
}

void UAct_AbilitySystemComponent::ProcessingInputDataComplete(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{
}

bool UAct_AbilitySystemComponent::ChekcInputLengthToSetInputLock(float InputLength,const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag)
{//如果输入时间超过缓冲时间则设置输入锁。
	if (InputLength>=AbilityInputBuffTime)
	{
		return false;
	}
	return true;
}

void UAct_AbilitySystemComponent::SetInputLock(const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag)
{
	InputLock=true;
	InputTagsInbuff.Empty();
	if (InputLockDelegate.IsBound())
	{
		InputLockDelegate.Execute(ActionInstance,InputDataAsset,Inputtag);
	}
	
}

void UAct_AbilitySystemComponent::SetInputUnlock(const FInputActionInstance& ActionInstance,UInputDataAsset* InputDataAsset,FGameplayTag Inputtag)
{
	InputLock=false;
	if (InputLockDelegate.IsBound())
	{
		InputUnlockDelegate.Execute(ActionInstance,InputDataAsset,Inputtag);
	}
	
}

bool UAct_AbilitySystemComponent::ExeAbilityInputInfo(const TArray<FAbilityInputInfo>& InputTagsBuff,FAbilityInputInfo& OutInputInfo)
{	//根据权重来进行检查
	int32 ExeInputIndex=INDEX_NONE;
	if (InputTagsBuff.Num()<=0) return false;
	for (int index=0;FAbilityInputInfo InputInfo:InputTagsBuff)
	{
		if (ExeInputIndex==INDEX_NONE)
		{
			ExeInputIndex=index;
		}
		
		if (InputInfo.InputWeightType>InputTagsInbuff[ExeInputIndex].InputWeightType)
		{
			ExeInputIndex=index;
		}
		index++;
	}
		OutInputInfo=InputTagsInbuff[ExeInputIndex];
		return true;
}

void UAct_AbilitySystemComponent::CheckFinalInput()
{
	//对输入内容进行选择：
	FAbilityInputInfo FinalInputInfo;
	if (ExeAbilityInputInfo(InputTagsInbuff,FinalInputInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,FString::Printf(TEXT("%f"),GetWorld()->GetTime().GetRealTimeSeconds()));
 		InputExecuteDelegate.Execute(FinalInputInfo);
	}
	GetWorld()->GetTimerManager().ClearTimer(FinalInputHandle);
}

void UAct_AbilitySystemComponent::OnInputFinal(const FAbilityInputInfo& InputInfo)
{//TODO::进行对应内容的处理：
	GEngine->AddOnScreenDebugMessage(-1,0.5,FColor::Red,FString::Printf(TEXT("%s"),*InputInfo.InputTag.ToString()));
	if (InputInfo.InputTag==ActTagContainer::InputRelaxAttack)
	{
		
	}
	if (InputInfo.InputTag==ActTagContainer::InputHeavyAttack)
	{
		
	}
	if (InputInfo.InputTag==ActTagContainer::InputDefense)
	{
		
	}
	if (InputInfo.InputTag==ActTagContainer::InputRolling)
	{
		
	}
}



