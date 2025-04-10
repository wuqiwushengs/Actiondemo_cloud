// Fill out your copyright notice in the Description page of Project Settings.


#include "ExePreInputNotify.h"

#include "actiondemo/Character/Act_Character.h"

void UExePreInputNotify::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	
}

void UExePreInputNotify::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	
}

void UExePreInputNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration,EventReference);
	AAct_Character* Character = Cast<AAct_Character>(MeshComp->GetOwner());
	if(!Character) return;
	Character->GetAct_AbilitySystemComponent()->AddLooseGameplayTag(ActTagContainer::ExePreInputRelaxAttack);
}

void UExePreInputNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);
	AAct_Character* Character=Cast<AAct_Character>(MeshComp->GetOwner());
	if (!Character)return;
	Character->GetAct_AbilitySystemComponent()->RemoveLooseGameplayTag(ActTagContainer::ExePreInputRelaxAttack);
}
