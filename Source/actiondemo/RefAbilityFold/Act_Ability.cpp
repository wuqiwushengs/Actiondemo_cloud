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
#pragma  region initAbility
	PreMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PreMontage,1.f,NAME_None,1.f);
	PreMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
	PreMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::PreHandleMontageBlendout);
	if (!NormalPostMontage)
	{
		PreMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
	}
	check(PreMontageTask);
	//技能蓄力阶段的任务
	if (bHoldMontage)
	{
		if (PreHoldMontage)
		{
			HoldMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PreHoldMontage,1.f,NAME_None,1.f);
			HoldMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
			HoldMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::TurnTohold);
			check(HoldMontage)
			
		}
		if (!PreHoldMontage&&HoldMontage)
		{
			HoldMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,HoldMontage,1.f,NAME_None,1.f);
			HoldMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
			check(HoldMontage)
		}
		
	}
	
	//当允许连续打击时创建这个任务
	if (bIsContinueMontage)
	{	//当有任务的时候就播放 TODO::需要处理所有动画在受伤时被打断后的输入。（额不对貌似不用只用在手上这里也添加即可）
	
		ContinueMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,ContinueMontage,1.f,NAME_None,1.f);
		//当动画混出的时候则播放后摇
		ContinueMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::TurnToPostMontage);
		//当动画被插入的时候则先处理蒙太奇混出
		ContinueMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
		check(ContinueMontage);
	}
	if(bIsContinueMontage)
	{    
		ContinueTagTask=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ActTagContainer::ExeMulityInputRelaxAttack);
		ContinueTagTask->EventReceived.AddDynamic(this,&UAct_Ability::OnContinueTagReceived);
		ContinueTagTask->Activate();
	}
	if (bHoldMontage)
	{	
		EventPressed=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ActTagContainer::Pressed);
		EventPressed->EventReceived.AddDynamic(this,&UAct_Ability::UAct_Ability::OnPressed);
		EventPressed->Activate();
		EventOnRealsed=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ActTagContainer::ExeHoldAbilityInputRelaxAttackReleased);
		EventOnRealsed->EventReceived.AddDynamic(this,&UAct_Ability::OnHoldEnded);
		EventOnRealsed->Activate();
	}
#pragma  endregion initAbility
}
void UAct_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo)) EndAbility(Handle,ActorInfo,ActivationInfo,true,false);\
	//技能前摇
	if (bUseSkillContext())
	{
		PreMontageTask->Activate();
		OnPreAnimPresssed();
		UE_LOG(LogTemp,Warning,TEXT("Ability"))
	}
	
}
void UAct_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
void UAct_Ability::OnEndAbility()
{	//绑定结束事件
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}

bool UAct_Ability::bUseSkillContext_Implementation()
{
	return true;
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
		if (!bIsPressed)
		{
			bIsPressed=true;
		}
	}
}
void UAct_Ability::TurnToPostMontage()
{	//当为真时则继续播放攻击动画
	
	if (bIsPressed)
	{	
		ContinueMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,ContinueMontage,1.f,NAME_None,1.f);
		//当动画混出的时候则播放后摇
		ContinueMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::TurnToPostMontage);
		//当动画被插入的时候则先处理蒙太奇混出
		ContinueMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
		ContinueMontageTask->Activate();
		OnContinuePressed();
		bIsPressed=false;
		return;
		
	}
	//当不是连打动画时则播放后摇
	if (PostContinueMontage)
	{	PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PostContinueMontage,1.f);
		PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->Activate();
		OnPostPressed();
	}
}
void UAct_Ability::OnContinuePressed()
{
}
#pragma endregion Continue
#pragma region Pre
void UAct_Ability::OnPressed(FGameplayEventData Data)
{
	if (!BExexute)
	{
		holdtime+=GetWorld()->GetDeltaSeconds();
	}
	
}

//处理动画前摇的动画混出
void UAct_Ability::PreHandleMontageBlendout()
{	
	
	if (!BExexute&&bHoldMontage&&HoldMontage&&holdtime>(PreMontage->GetPlayLength()-0.3))
	{	//当判断有蓄力动画并且时间大于前摇动画的时间时，播放蓄力动画
		HoldMontageTask->Activate();
		OnHoldPressed();
		return;
	}
	//当不是蓄力动画或者蓄力动画没有到时间但是是连打动画是允许的话就播放连打动画
	if (bIsContinueMontage&&bIsPressed)
	{	
		ContinueMontageTask->Activate();
		OnContinuePressed();
		BExexute=true;
		bIsPressed=false;
		return;
	}
	if (NormalPostMontage)
	{	//当判断不是蓄力动画和连打动画，则播放后摇动画
		PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,NormalPostMontage,1.f);
		PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
		PostMontageTask->Activate();
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
	int32 level=FMath::Clamp(FMath::FloorToInt32(holdtime/HoldUpLevelTime),0,HoldPostMontage.Num()-1);
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
		holdtime=0;
		//当松开时，播放post内容结束后摇随后结束技能
		if (Montage)
		{
			PostMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f);
			PostMontageTask->OnBlendOut.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnCompleted.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnCancelled.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::OnEndAbility);
			PostMontageTask->Activate();
			OnPostPressed();
		}
		
		
		
	}
	
}

void UAct_Ability::OnHoldPressed()
{
}

void UAct_Ability::TurnTohold()
{
	HoldMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,HoldMontage,1.f,NAME_None,1.f);
	HoldMontageTask->OnInterrupted.AddDynamic(this,&UAct_Ability::HandleMontageInterrupted);
	check(HoldMontage)
	HoldMontageTask->Activate();
	
}
#pragma endregion Hold
//动画被插入时的处理

