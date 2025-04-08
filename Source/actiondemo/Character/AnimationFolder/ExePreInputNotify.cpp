// Fill out your copyright notice in the Description page of Project Settings.


#include "ExePreInputNotify.h"

#include "actiondemo/Character/Act_Character.h"

void UExePreInputNotify::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	AAct_Character* Character = Cast<AAct_Character>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if(!Character) return;
	Character->GetAct_AbilitySystemComponent()->AddLooseGameplayTag(ActTagContainer::ExePreInputRelaxAttack);
}

void UExePreInputNotify::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	AAct_Character* Character=Cast<AAct_Character>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (!Character)return;
	Character->GetAct_AbilitySystemComponent()->RemoveLooseGameplayTag(ActTagContainer::ExePreInputRelaxAttack);
}
