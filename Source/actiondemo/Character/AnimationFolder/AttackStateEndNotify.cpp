// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStateEndNotify.h"
#include "actiondemo/Character/Act_Character.h"

void UAttackStateEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetCharacterAttackingState_Implementation(ECharacterState::UnAttacking);
	}
}
