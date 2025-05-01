// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilitySystemComponent.h"
#include "actiondemo/InputDataAsset.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagBase.h"
#include "actiondemo/Character/Act_Character.h"
#include "actiondemo/Character/CharacterInferface.h"
#include "AttributeContent/Act_AttributeSet.h"


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
	this->RegisterGameplayTagEvent(ActTagContainer::ExePreInputRelaxAttack,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UAct_AbilitySystemComponent::OnPreSkillExecute);
}
void UAct_AbilitySystemComponent::ProcessingInputDataStarted(const FInputActionInstance& ActionInstance,FGameplayTag Inputag, UInputDataAsset* InputDataAsset)
{	
	FInputData InputData=InputDataAsset->GetAbilityInputDatabyTag(Inputag);
	float WordTime=ActionInstance.GetLastTriggeredWorldTime();
	
	//检测当前是预输入阶段还是普通输入阶段，如果是预输入阶段那么就走下面，如果是普通输入阶段则走下面这一段。
	switch (CurrentInputType)
	{
	case  InputState::PreInputState:
		{	//TODO::优化操作手感，感觉这里经常会卡手，不容易连击，需要调整。（暂时应该不用更改）
			if (!CheckIsAllowed(Inputag)) break;
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
				FinalExecute.BindLambda([this,ActionInstance,InputDataAsset,index](){SetInputLock(ActionInstance,InputDataAsset,this->InputTagsInbuff,index);CheckFinalInput();});
				GetWorld()->GetTimerManager().SetTimer(FinalInputHandle,FinalExecute,AbilityInputBuffTime,false);
			}
			break;
		}
		
	case InputState::DisableInputState:
		{	//用来处理在输入锁定后连续打击类型的技能
			//需要加一个判断，假如当前的输入tag是预输入阶段的tag那么就执行
			if (GetActivatableAbilities().IsEmpty())
			{
				SetInputstate(InputState::NormalInputState);
				ProcessingInputDataStarted(ActionInstance, Inputag, InputDataAsset);
				UE_LOG(LogTemp,Warning,TEXT("Disable to Normal"))
			}
			InputTagsInbuff.Empty();
			if (Inputag==AbilityChainManager->CurrentAbilityType.InputTag)
			{
				FGameplayEventData EventData;
				EventData.Instigator=GetOwner();
				EventData.EventTag=Inputag;
				check(GetOwner());
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
	if (AbilityChainManager->UnComboHandle.Find(Inputag))
	{		//处理用sendgameplaytag;
		UE_LOG(LogTemp,Warning,TEXT("TRY Continue"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::ExeUnComboAbilityInputReleased,FGameplayEventData());
	}
	//无法解决快速连按的问题，会出现无法进入的情况，只能慢速
	if (AbilityChainManager->CurrentAbilityType.InputTag==Inputag)
	{	FGameplayEventData EventData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::ExeHoldAbilityInputRelaxAttackReleased,EventData);
	}
	if (Inputag==ActTagContainer::InputDefense)
	{
		RemoveLooseGameplayTag(ActTagContainer::InputDefense);
		
	}
	TriggerTag=FGameplayTag();
	
}

void UAct_AbilitySystemComponent::ProcessingInputDataTrigger(const FInputActionInstance& ActionInstance,
	FGameplayTag Inputag, UInputDataAsset* InputDataAsset,float TriggerTime)
{	//当单机时设置
	FGameplayAbilitySpecHandle Handle;
	if (AbilityChainManager->CurrentAbilityType.Handle.IsValid())
	{	
		Handle=AbilityChainManager->CurrentAbilityType.Handle;
		check(Handle.IsValid());
		if (Handle.IsValid()&&InputDataAsset->GetAbilityInputDatabyTag(Inputag).bCanHold&&Inputag==ActTagContainer::InputHeavyAttack)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ActTagContainer::Pressed,FGameplayEventData());
			
		}
	}
	TriggerTag=Inputag;
}
bool UAct_AbilitySystemComponent::ChekcInputLengthToSetInputLock(float InputLength,const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,FGameplayTag Inputtag)
{//如果输入时间超过缓冲时间则设置输入锁。
	if (InputLength>=AbilityInputBuffTime)
	{
		return false;
	}
	return true;
}

void UAct_AbilitySystemComponent::SetInputLock(const FInputActionInstance & ActionInstance,UInputDataAsset *InputDataAsset,TArray<FAbilityInputInfo> & Content,int32 index)
{

	if (Content.Num()-1<index)
	{
		InputTagsInbuff.Empty();
		GetWorld()->GetTimerManager().ClearTimer(FinalInputHandle);
		return;
	}
	CurrentInputType=InputState::DisableInputState;
	FGameplayTag InputTag=Content[index].InputTag;
	if (InputLockDelegate.IsBound())
	{
		InputLockDelegate.Execute(ActionInstance,InputDataAsset, InputTag);
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
	InputTagsInbuff.Empty();
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
		if (AbilityDataManager->AbilityData->GetAbilityTypesNotInComboChainByTag(ActTagContainer::InputDefense,DefenseAbility))
		{
			this->TryActivateAbilityByClass(DefenseAbility.Ability);
		}
	}
	/*if (InputInfo.InputTag==ActTagContainer::InputRolling)
	{
		//激活翻滚技能
		FAct_AbilityTypes RollingAbility;
		if (AbilityDataManager->AbilityData->GetAbilityTypesNotInComboChainByTag(ActTagContainer::InputRolling,RollingAbility))
		{
			if (!this->TryActivateAbilityByClass(RollingAbility.Ability))
			{
				AAct_Character * Character=Cast<AAct_Character>(GetOwner());
				Character->SetCharacterAttackingState_Implementation(ECharacterState::UnAttacking);
				
			}
			
		}
	}*/
}
void UAct_AbilitySystemComponent::SetInputDisable(const FGameplayTagContainer& DisableTag)
{	TArray<FGameplayTag> DisAbleTags=DisableTag.GetGameplayTagArray();
	for (FGameplayTag  Tag:DisAbleTags)
	{
		if (AllowedPreInput.Find(Tag))
		{
			AllowedPreInput[Tag]=0;
		}
	}
}
void UAct_AbilitySystemComponent::TurnPreInputToDefault()
{
	for (TPair<FGameplayTag,int>&Tag:AllowedPreInput)
	{
		Tag.Value=1;
	}
}
bool UAct_AbilitySystemComponent::CheckIsAllowed(FGameplayTag PreInputTag)
{
	if (AllowedPreInput.Find(PreInputTag))
	{
		return  AllowedPreInput[PreInputTag]==1;
	}
	return false;
}
void UAct_AbilitySystemComponent::SetInputstate(InputState InputType)
{
	this->CurrentInputType=InputType;
	if (InputType==InputState::NormalInputState)
	{
		AbilityChainManager->SelectedNode=nullptr;
		UAct_AbilityChainChildNode * Node=AbilityChainManager->SelectedNode;
		AAct_Character * player=Cast<AAct_Character>(GetOwner());
		UE_LOG(LogTemp,Warning,TEXT("%s"),*TriggerTag.ToString());
		if (TriggerTag.IsValid()&&TriggerTag==ActTagContainer::InputHeavyAttack&&player&&!GetOwnedGameplayTags().HasTagExact(ActTagContainer::WeakState))
		{
			if (AbilityChainManager->ToNextNode(Node,EAttackType::HeavyAttack,player->GetCharacterUnAttackingState_Implementation()))
			{
				SetInputstate(InputState::DisableInputState);
				InputTagsInbuff.Empty();
			
			}
		}
		if (TriggerTag==ActTagContainer::InputDefense&&!GetOwnedGameplayTags().HasTagExact(ActTagContainer::WeakState))
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),TriggerTag,FGameplayEventData());
			InputTagsInbuff.Empty();
		}
		InputTagsInbuff.Empty();
	}
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
			UE_LOG(LogTemp,Warning,TEXT("PRE"))
		}
		CurrentInputType=InputState::DisableInputState;
		InputTagsInbuff.Empty();
	}
}



