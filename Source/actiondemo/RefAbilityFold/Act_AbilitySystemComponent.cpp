﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilitySystemComponent.h"
#include "actiondemo/InputDataAsset.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagBase.h"
#include "actiondemo/Character/CharacterInferface.h"


UAct_AbilitySystemComponent::UAct_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentInputType=InputState::NormalInputState;
	//初始化
	AbilityDataManager=CreateDefaultSubobject<UAct_AbilityDatasManager>("AbilityData");
	AbilityChainManager=CreateDefaultSubobject<UAct_AbilityChainManager>("AbilityChainManager");
}
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
		AbilityChainManager->BeginConstruct(AbilityDataManager,this);
	}
	//绑定输入执行函数
	InputExecuteDelegate.BindUFunction(this,TEXT("OnInputFinal"));
	this->RegisterGameplayTagEvent(ActTagContainer::ExePreInputRelaxAttack,EGameplayTagEventType::NewOrRemoved);
}
void UAct_AbilitySystemComponent::ProcessingInputDataStarted(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{
	FInputData InputData=InputDataAsset->GetAbilityInputDatabyTag(Inputag);
	float WordTime=ActionInstance.GetLastTriggeredWorldTime();
	
	//检测当前是预输入阶段还是普通输入阶段，如果是预输入阶段那么就走下面，如果是普通输入阶段则走下面这一段。
	switch (CurrentInputType)
	{
	case  InputState::PreInputState:
		{
			FAbilityInputInfo Abilityinfo(Inputag,WordTime,InputTagsInbuff.Num()<=0?0:WordTime-InputTagsInbuff[0].InputWordTime,InputData.InputType);
			InputTagsInbuff.Add(Abilityinfo);
			if (this->GetOwnedGameplayTags().HasTag(ActTagContainer::ExePreInputRelaxAttack))
			{
				//如果当前的输入tag是预输入阶段的tag那么就直接返回
				FAbilityInputInfo FinalInputInfo;
				if (ExeAbilityInputInfo(InputTagsInbuff,FinalInputInfo))
				{
					InputExecuteDelegate.Execute(FinalInputInfo);
				}
				CurrentInputType=InputState::DisableInputState;
				InputTagsInbuff.Empty();
			}
			break;
		}
	case InputState::NormalInputState:
		{
			FAbilityInputInfo Abilityinfo(Inputag,WordTime,InputTagsInbuff.Num()<=0?0:WordTime-InputTagsInbuff[0].InputWordTime,InputDataAsset->GetAbilityInputDatabyTag(Inputag).InputType);
			if (ChekcInputLengthToSetInputLock(Abilityinfo.InputIntervalTime,ActionInstance,InputDataAsset,Inputag))
			{
				InputTagsInbuff.Add(Abilityinfo);
			}
			if (InputTagsInbuff.Num()>=1&&!GetWorld()->GetTimerManager().IsTimerActive(FinalInputHandle))
			{
				//假如已经绑定了那就不再进行绑定
				int32 index=this->InputTagsInbuff.Num()-1;
				FTimerDelegate FinalExecute;
				FinalExecute.BindLambda([this,ActionInstance,InputDataAsset,index](){CheckFinalInput();SetInputLock(ActionInstance,InputDataAsset,this->InputTagsInbuff[index].InputTag);});
				GetWorld()->GetTimerManager().SetTimer(FinalInputHandle,FinalExecute,AbilityInputBuffTime,false);
			}
			break;
		}
		
	case InputState::DisableInputState:
		{	//用来处理在输入锁定后连续打击类型的技能
			//需要加一个判断，假如当前的输入tag是预输入阶段的tag那么就执行
			if (Inputag==AbilityChainManager->CurrentAbilityType.InputTag)
			{
				FGameplayEventData EventData;
				EventData.Instigator=GetOwner();
				EventData.EventTag=Inputag;
				check(GetOwner());
				/*UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::ExeMulityInputRelaxAttack,EventData);*/
				if (FGameplayEventMulticastDelegate* Delegate = GenericGameplayEventCallbacks.Find(ActTagContainer::ExeMulityInputRelaxAttack))
				{
					// Make a copy before broadcasting to prevent memory stomping
					FGameplayEventMulticastDelegate DelegateCopy = *Delegate;
					DelegateCopy.Broadcast(&EventData);
				}
			}
		}
	}
}
void UAct_AbilitySystemComponent::ProcessingInputDataComplete(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{	//检查获取取消键的技能的委托并且释放。保证不会因为一个导致其他的技能被释放
	
	FAct_AbilityTypes NotInComboSkill;
	FGameplayAbilitySpecHandle Handle;
	FInputData InputData=InputDataAsset->GetAbilityInputDatabyTag(Inputag);
	if (AbilityChainManager->UnComboHandle.Find(Inputag))
	{		//处理用sendgameplaytag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::ExeUnComboAbilityInputReleased,FGameplayEventData());
	}
	
	if (AbilityChainManager->CurrentAbilityType.InputTag==Inputag&&ActionInstance.GetElapsedTime()>0.5)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::ExeHoldAbilityInputRelaxAttackReleased,FGameplayEventData());
		CurrentHoldTime=0.0f;
	}
}

void UAct_AbilitySystemComponent::ProcessingInputDataTrigger(const FInputActionInstance& ActionInstance,
	FGameplayTag Inputag, UInputDataAsset* InputDataAsset,float TriggerTime)
{	
	FGameplayAbilitySpecHandle Handle;
	if (AbilityChainManager->UnComboHandle.Find(Inputag))
	{
		Handle=*AbilityChainManager->UnComboHandle.Find(Inputag);
		check(Handle.IsValid());
		if (Handle.IsValid()&&InputDataAsset->GetAbilityInputDatabyTag(Inputag).bCanHold)
		{
			bool BInstance=false;
			if (const UAct_Ability * Ability=Cast<UAct_Ability>(UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(this,Handle,BInstance)))
			{
				IIAct_AbilityInterface::Execute_SetTriggerTime(const_cast<UAct_Ability*>(Ability),TriggerTime);
				
				
			}
		}
	}
	if (AbilityChainManager->CurrentAbilityType.Handle.IsValid())
	{
		Handle=AbilityChainManager->CurrentAbilityType.Handle;
		check(Handle.IsValid());
		if (Handle.IsValid()&&InputDataAsset->GetAbilityInputDatabyTag(Inputag).bCanHold)
		{
			bool BInstance=false;
			CurrentHoldTime=TriggerTime;
			
		}
	}
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
	CurrentInputType=InputState::DisableInputState;
	InputTagsInbuff.Empty();
	if (InputLockDelegate.IsBound())
	{
		InputLockDelegate.Execute(ActionInstance,InputDataAsset,Inputtag);
	}
	
}

void UAct_AbilitySystemComponent::SetInputUnlock()
{
	CurrentInputType=InputState::NormalInputState;
	if (InputLockDelegate.IsBound())
	{
		InputUnlockDelegate.Execute();
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
 		InputExecuteDelegate.Execute(FinalInputInfo);
	}
	GetWorld()->GetTimerManager().ClearTimer(FinalInputHandle);
}

void UAct_AbilitySystemComponent::OnInputFinal(const FAbilityInputInfo& InputInfo)
{//TODO::进行对应内容的处理：
	
	if (InputInfo.InputTag==ActTagContainer::InputRelaxAttack)
	{	UAct_AbilityChainChildNode * Temp=AbilityChainManager->SelectedNode.Get();
		AbilityChainManager->ToNextNode(Temp,EAttackType::RelaxAttack,ICharacterInferface::Execute_GetCharacterUnAttackingState(GetOwner()));
		
	}
	if (InputInfo.InputTag==ActTagContainer::InputHeavyAttack)
	{	UAct_AbilityChainChildNode * Temp=AbilityChainManager->SelectedNode.Get();
		AbilityChainManager->ToNextNode(Temp,EAttackType::HeavyAttack,ICharacterInferface::Execute_GetCharacterUnAttackingState(GetOwner()));
	}
	if (InputInfo.InputTag==ActTagContainer::InputDefense)
	{
		//激活防御技能
		FAct_AbilityTypes DefenseAbility;
		if (DefenseAbility.InputTag.IsValid()&&AbilityDataManager->AbilityData->GetAbilityTypesNotInComboChainByTag(ActTagContainer::InputDefense,DefenseAbility))
		{
			this->TryActivateAbilityByClass(DefenseAbility.Ability);
		}
	}
	if (InputInfo.InputTag==ActTagContainer::InputRolling)
	{
		//激活翻滚技能
		FAct_AbilityTypes RollingAbility;
		if (RollingAbility.InputTag.IsValid()&&AbilityDataManager->AbilityData->GetAbilityTypesNotInComboChainByTag(ActTagContainer::InputRolling,RollingAbility))
		{
			this->TryActivateAbilityByClass(RollingAbility.Ability);
		}
	}
}

void UAct_AbilitySystemComponent::SetInputstate(InputState InputType)
{
	this->CurrentInputType=InputType;
}

void UAct_AbilitySystemComponent::OnPreSkillExecute(const FGameplayTag ExeTag, int32 count)
{	//假如当前的技能预输入数量小于等于0那么就直接返回
	if (InputTagsInbuff.Num()<=0) return ;
	if (count>=1&&ExeTag==ActTagContainer::ExePreInputRelaxAttack)
	{
		//如果计数>=1说明进入了执行预输入阶段。
		FAbilityInputInfo FinalInputInfo;
		if(ExeAbilityInputInfo(InputTagsInbuff,FinalInputInfo))
		{
			InputExecuteDelegate.Execute(FinalInputInfo);
		}
		CurrentInputType=InputState::DisableInputState;
		InputTagsInbuff.Empty();
	}
}



