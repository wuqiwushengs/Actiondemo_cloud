// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackResultSingleCheck.h"
#include "actiondemo/Character/Act_Character.h"


void UAttackResultSingleCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner()))
	{
		for (TPair<FName,FLineTraceInfo>& BoneInfo:Character->TraceBoneAndInfo)
		{
			BoneInfo.Value.InitLocation();
			UE_LOG(LogTemp,Warning,TEXT("Current %s"),*BoneInfo.Value.CurrenLocation.ToString());
			UE_LOG(LogTemp,Warning,TEXT("Last  %s"),*BoneInfo.Value.CurrenLocation.ToString());
		}
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
