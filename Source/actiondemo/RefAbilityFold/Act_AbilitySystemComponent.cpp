﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilitySystemComponent.h"



// Sets default values for this component's properties
UAct_AbilitySystemComponent::UAct_AbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAct_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAct_AbilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	
	
	
}
// @TODO::解决输入之后并没有完成添加的问题；
void UAct_AbilitySystemComponent::ProcessingInputDataStarted(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{  //获得当前所有输入的tag以及输入的世界时间
	//处理输入缓存设置
	const FInputData & Data=InputDataAsset->GetAbilityInputDatabyTag(Inputag);
	float WordTime=ActionInstance.GetLastTriggeredWorldTime();
	FAbilityInputInfo Abilityinfo(Inputag,WordTime,InputTagsInbuff.Num()<=0?0:WordTime-InputTagsInbuff[0].InputWordTime,InputDataAsset->GetAbilityInputDatabyTag(Inputag).InputType);
	if (ChekcInputLengthToSetInputLock(Abilityinfo.InputIntervalTime,ActionInstance,InputDataAsset,Inputag)) InputTagsInbuff.Add(Abilityinfo);
	//假如已经绑定了那就不再进行绑定
	if (!FinalInputHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(FinalInputHandle,this,&UAct_AbilitySystemComponent::CheckFinalInput,AbilityInputBuffTime,false);
	}
	
	
	
	
	
	
	
}

void UAct_AbilitySystemComponent::ProcessingInputDataTriggering(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{
}

void UAct_AbilitySystemComponent::ProcessingInputDataComplete(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{
}

bool UAct_AbilitySystemComponent::ChekcInputLengthToSetInputLock(float InputLength,const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag)
{
	if (InputLength>=AbilityInputBuffTime)
	{
		SetInputLock(ActionInstance,InputDataAsset,Inputtag);
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
{
	int32 ExeInputIndex=INDEX_NONE;
	if (InputTagsBuff.Num()<=0) return false;
	for (int index=0;FAbilityInputInfo InputInfo:InputTagsBuff)
	{
		if (ExeInputIndex==INDEX_NONE)	ExeInputIndex=index;
		if (InputInfo.InputWeightType>InputTagsInbuff[ExeInputIndex].InputWeightType)
		{
			ExeInputIndex=index;
		}
	}
	if (ExeInputIndex!=INDEX_NONE)
	{	OutInputInfo=InputTagsInbuff[ExeInputIndex];
		return true;
	}
	return  false;
}

void UAct_AbilitySystemComponent::CheckFinalInput()
{
	//对输入内容进行选择：
	FAbilityInputInfo FinalInputInfo;
	if (ExeAbilityInputInfo(InputTagsInbuff,FinalInputInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Black,FString::Printf(TEXT("FinalInput %s"),*FinalInputInfo.InputTag.ToString()));
	}
	GetWorld()->GetTimerManager().ClearTimer(FinalInputHandle);
}



