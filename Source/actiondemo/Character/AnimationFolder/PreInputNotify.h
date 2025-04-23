// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "PreInputNotify.generated.h"

/**
 * 用于在技能动画中开启预输入状态的通知
 */
UCLASS()
class ACTIONDEMO_API UPreInputNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual FLinearColor GetEditorColor() override;
	virtual  void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//用来检测当前输入时什么预输入是不允许的。
	UPROPERTY(EditAnywhere,meta=(ExposeOnSpawn),BlueprintReadOnly)
	FGameplayTagContainer DisablePreTag;
};
