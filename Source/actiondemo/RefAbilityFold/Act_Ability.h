// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityInterFace/IAct_AbilityInterface.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Act_Ability.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
DECLARE_MULTICAST_DELEGATE(FOnpressedDelegate)
/**
 * 该部分中将一个动画分为前摇和后摇，蓄力阶段，连打阶段四个部分，对于前摇和后摇并没有分太细。
 */
UCLASS()
class ACTIONDEMO_API UAct_Ability : public UGameplayAbility,public IIAct_AbilityInterface
{
	GENERATED_BODY()
public:
	//如果是蓄力的话，则调用这个函数
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData*  TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UFUNCTION()
	void OnEndAbility();
	UFUNCTION()
	void OnAbilityTrigger(float AbilityTime=0);
	FOnpressedDelegate OnRealesedDelegate;
#pragma region TimeInformation
	UPROPERTY()
	float AbilityTriggerTime;
	UPROPERTY()
	bool bIsTriggerTimeSet;
	UPROPERTY(EditDefaultsOnly)
	float AbilityMinHoldTime=0.5f;
	virtual void SetTriggerTime_Implementation(float TriggerTime) override;
#pragma endregion
#pragma region Animation
#pragma  region Pre
	//技能前摇或者是单一一个技能的任务
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PreMontageTask;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* PreMontage;
	UFUNCTION()
	void PreHandleMontageBlendout();
#pragma  endregion pre
#pragma region Hold
	//技能蓄力阶段的任务：需要判断是否是蓄力
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* HoldMontageTask;
	//如果不需要蓄力则不填即可，但这个取决于之前设置的这个键位是否有蓄力能力，假如有蓄力能力则最好填上，当然也可以逻辑里自己调整
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HoldMontage;
	//按下多久开启下一个等级的蓄力。后面可能会扩展
	UPROPERTY()
	float HoldUpLevelTime;
	UFUNCTION()
	void OnHoldEnded();
#pragma endregion Hold
#pragma region Continue
	//技能在连续打击阶段的任务：需要判断是否是连续打击
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* ContinueMontageTask;
	//等待连续打击的tag
	UPROPERTY()
	UAbilityAsync_WaitGameplayEvent* ContinueTagTask;
	//连续打击的蒙太奇不是循环蒙太奇
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bisContinueMontage"))
	UAnimMontage* ContinueMontage;
	UPROPERTY()
	bool bIsPressed;
	//如果这个布尔为真的话那么一定要有动画，一定要有动画，不然会崩溃的孩子，懒得加检查了。
	UPROPERTY(EditDefaultsOnly)
	bool bIsContinueMontage;
	UFUNCTION()
	void OnContinueTagReceived (FGameplayEventData EventData);
	UFUNCTION()
	void TurnToPostMontage();
#pragma endregion Continue
#pragma region post
	//技能后摇阶段的任务：需要判断是否有后摇
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PostMontageTask;
	UPROPERTY(EditDefaultsOnly)
	TMap<int32,UAnimMontage*>HoldPostMontage;
	//这个是必填的这个是假如单击的话他的后摇动画是什么。
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* NormalPostMontage;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* PostContinueMontage;
	//保证Interupt安全的方式
	UFUNCTION()
	void HandleMontageInterrupted();
	UFUNCTION()
	int32 CaculateAbilityHoldLevel(float HoldLevelTime ) const ;
#pragma endregion post
#pragma endregion Animation	
	
	
};
