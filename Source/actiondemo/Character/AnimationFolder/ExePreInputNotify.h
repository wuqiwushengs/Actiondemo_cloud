// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ExePreInputNotify.generated.h"

/**
*
* 注意：该通知的执行需要与`PreInputNotify`保持末尾时间上的完全重合，否则可能导致该通知无法正常触发或使用。
 */
UCLASS()
class ACTIONDEMO_API UExePreInputNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual  void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
