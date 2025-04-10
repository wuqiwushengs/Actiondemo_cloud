// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Ability.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "actiondemo/Act_TagContainer.h"

class UAbilityAsync_WaitGameplayEvent;
//TODO::连击的测试。
void UAct_Ability::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	//技能前摇以及单个技能任务的创建
	
	PreMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PreMontage,1.f,NAME_None,1.f);
	PreMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
	PreMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::PreHandleMontageBlendout);
	if (!NormalPostMontage)
	{
		PreMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
	}
	PreMontageTask->ReadyForActivation();
	check(PreMontageTask);
	//技能蓄力阶段的任务
	if (HoldMontage)
	{
		HoldMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,HoldMontage,1.f,NAME_None,1.f);
		HoldMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
		HoldMontageTask->ReadyForActivation();
		check(HoldMontage)
	}
	
	//当允许连续打击时创建这个任务
	if (bIsContinueMontage)
	{	//当有任务的时候就播放 TODO::需要处理所有动画在受伤时被打断后的输入。（额不对貌似不用只用在手上这里也添加即可）
	
		ContinueMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,ContinueMontage,1.f,NAME_None,1.f);
		//当动画混出的时候则播放后摇
		ContinueMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::TurnToPostMontage);
		//当动画被插入的时候则先处理蒙太奇混出
		ContinueMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
		ContinueMontageTask->ReadyForActivation();
		check(ContinueMontage);
	}
	
}
void UAct_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo)) EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	if(bIsContinueMontage)
	{    
		ContinueTagTask=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ActTagContainer::ExeMulityInputRelaxAttack);
		ContinueTagTask->EventReceived.AddDynamic(this,&UAct_Ability::OnContinueTagReceived);
		ContinueTagTask->Activate();
		
	}
	if (HoldMontage)
	{
		EventOnRealsed=UAbilityAsync_WaitGameplayEvent::WaitGameplayEventToActor(GetOwningActorFromActorInfo(),ActTagContainer::ExeHoldAbilityInputRelaxAttackReleased,false,true);
		EventOnRealsed->EventReceived.AddDynamic(this,&UAct_Ability::OnHoldEnded);
	}
	//技能前摇
	PreMontageTask->Activate();
	OnPreAnimPresssed();
}
void UAct_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
void UAct_Ability::OnEndAbility()
{	//绑定结束事件
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}
void UAct_Ability::SetTriggerTime_Implementation(float AbilityTime)
{
	this->AbilityTriggerTime=AbilityTime;
	OnAbilityTrigger(AbilityTriggerTime);
	
}
//按下时连续触发
void UAct_Ability::OnAbilityTrigger(float TriggerTime)
{
	//处理所有持续时间的技能的一些效果。
}
void UAct_Ability::HandleMontageInterrupted()
{	
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags().HasTag(ActTagContainer::ExeInterrupt))
	{
		OnEndAbility();
	}
}
#pragma region Continue
//当在输入锁定时按下时会触发这个事件
void UAct_Ability::OnContinueTagReceived(FGameplayEventData EventData)
{   if (EventData.EventTag==ActTagContainer::ExeMulityInputRelaxAttack&&bIsContinueMontage)
	{
		if (ContinueMontageTask->IsActive()&&!bIsPressed)
		{
			bIsPressed=true;
		}
		//如果是连续打击的tag则判断其什么时候执行
		if (!ContinueMontageTask)
		{
			ContinueMontageTask->Activate();
			OnContinuePressed();
			
		}
	}
}
void UAct_Ability::TurnToPostMontage()
{	//当为真时则继续播放攻击动画
	GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,TEXT("HELLO"));
	if (bIsPressed)
	{	ContinueMontageTask->EndTask();
		ContinueMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,ContinueMontage,1.f,NAME_None,1.f);
		//当动画混出的时候则播放后摇
		ContinueMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::TurnToPostMontage);
		//当动画被插入的时候则先处理蒙太奇混出
		ContinueMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
		ContinueMontageTask->ReadyForActivation();
		ContinueMontageTask->Activate();
		OnContinuePressed();
		bIsPressed=false;
		
	}
	//当不是连打动画时则播放后摇
	if (PostContinueMontage)
	{	PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PostContinueMontage,1.f);
		PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->ReadyForActivation();
		PostMontageTask->Activate();
		OnPostPressed();
	}
}
void UAct_Ability::OnContinuePressed()
{
}
#pragma endregion Continue
#pragma region Pre
//处理动画前摇的动画混出
void UAct_Ability::PreHandleMontageBlendout()
{	//通过减0.05的方式避免到这个时候判断时间差一丢丢不够，提前一两帧
	if (HoldMontage&&AbilityTriggerTime>PreMontage->GetPlayLength()-0.05)
	{	//当判断有蓄力动画并且时间大于前摇动画的时间时，播放蓄力动画
		HoldMontageTask->Activate();
		OnHoldPressed();
	}
	//当不是蓄力动画或者蓄力动画没有到时间但是是连打动画是允许的话就播放连打动画
	if (bIsContinueMontage)
	{
		ContinueMontageTask->Activate();
		OnContinuePressed();
	}
	if (NormalPostMontage)
	{	//当判断不是蓄力动画和连打动画，则播放后摇动画
		PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,NormalPostMontage,1.f);
		PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->ReadyForActivation();
		OnPostPressed();
	}
}
void UAct_Ability::OnPreAnimPresssed()
{
}
#pragma endregion pre
#pragma region Hold
int32 UAct_Ability::CaculateAbilityHoldLevel(float HoldLevelTime) const
{	
	int32 level=FMath::Clamp(int32(AbilityTriggerTime/HoldLevelTime)-1,0,HoldPostMontage.Num()-1);
	return level;
}

void UAct_Ability::OnPostPressed()
{
}

void UAct_Ability::OnHoldEnded(FGameplayEventData EventData)
{
	if (!BExexute)
	{
		UAnimMontage* Montage=nullptr;
		BExexute=true;
		if (!HoldPostMontage.IsEmpty())
		{
			Montage=HoldPostMontage[CaculateAbilityHoldLevel(HoldUpLevelTime)];
		}
	
		//当松开时，播放post内容结束后摇随后结束技能
	
		if (Montage&&AbilityTriggerTime>PreMontage->GetPlayLength()+1)
		{
			PostMontageTask->EndTask();
			PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f);
			PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->ReadyForActivation();
			PostMontageTask->Activate();
			OnPostPressed();
		}
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,TEXT("helloworld"));
		
	}
	
}

void UAct_Ability::OnHoldPressed()
{
}
#pragma endregion Hold
//动画被插入时的处理

