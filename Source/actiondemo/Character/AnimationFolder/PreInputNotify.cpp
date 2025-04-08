// Fill out your copyright notice in the Description page of Project Settings.


#include "PreInputNotify.h"
#include "actiondemo/Character/Act_Character.h"
void UPreInputNotify::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	AAct_Character * Character=Cast<AAct_Character>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (!Character)return;
	Character->ActAbilitySystemComponent->SetInputstate(InputState::PreInputState);
}

void UPreInputNotify::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	AAct_Character * Character=Cast<AAct_Character>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (!Character)return;
	Character->ActAbilitySystemComponent->SetInputstate(InputState::DisableInputState);
	
}
