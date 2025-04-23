// Fill out your copyright notice in the Description page of Project Settings.


#include "PreInputNotify.h"
#include "actiondemo/Character/Act_Character.h"

FLinearColor UPreInputNotify::GetEditorColor()
{
	return FLinearColor::Yellow;
}

void UPreInputNotify::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	
}

void UPreInputNotify::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
}

void UPreInputNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration,EventReference);
	AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner());
	if (!Character)return;
	Character->ActAbilitySystemComponent->SetInputstate(InputState::PreInputState);
	Character->ActAbilitySystemComponent->SetInputDisable(DisablePreTag);
}

void UPreInputNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);
	AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner());
	if (!Character)return;
	Character->ActAbilitySystemComponent->TurnPreInputToDefault();
}
