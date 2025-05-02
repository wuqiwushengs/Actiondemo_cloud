// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackResultSingleCheck.h"
#include "actiondemo/Character/Act_Character.h"


void UAttackResultSingleCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner()))
	{
		Character->SingleBoneAndInfo.begin().Value().InitLocation();
		Character->AttackedActor.Empty();
		Character->PlayerHitResult.Empty();
	}
}

void UAttackResultSingleCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner()))
	{
		Character->TryAttackTrace(false);
	}
}
