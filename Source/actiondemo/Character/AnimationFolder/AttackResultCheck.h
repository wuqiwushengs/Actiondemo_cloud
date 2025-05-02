// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "actiondemo/Character/Act_Character.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackResultCheck.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAttackResultCheck : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//TODO 有一个单体方向的需要进行判断，这里只有刀的检测，有些地方是某些特定骨骼的检测。
};
