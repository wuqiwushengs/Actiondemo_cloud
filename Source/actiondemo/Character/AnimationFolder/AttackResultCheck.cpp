// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackResultCheck.h"
void UAttackResultCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner()))
	{
		Character->TryAttackTrace(true);
	}

	
}

void UAttackResultCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner()))
	{
		for (TPair<FName,FLineTraceInfo>& BoneInfo:Character->TraceBoneAndInfo)
		{
			BoneInfo.Value.InitLocation();
		}
		Character->AttackedActor.Empty();
		Character->PlayerHitResult.Empty();
	}
}
