// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PreInputNotify.generated.h"

/**
 * 用于在技能动画中开启预输入状态的通知
 */
UCLASS()
class ACTIONDEMO_API UPreInputNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual  void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
