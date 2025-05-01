// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AddorRemoveTag.h"

#include "actiondemo/Character/Act_Character.h"
#include "actiondemo/Character/CharacterTypes.h"

void UAN_AddorRemoveTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	 AAct_Character * Character=Cast<AAct_Character>( MeshComp->GetOwner());
	if (Character)
	{
		UAct_AbilitySystemComponent * AbilitySystemComponent= Character->GetAct_AbilitySystemComponent();
		switch (ApplyTagState)
		{
		case  EAddOrRemove::Add:
			{
				if (!AbilitySystemComponent->GetOwnedGameplayTags().HasTagExact(ApplyTag))
				{
					AbilitySystemComponent->AddLooseGameplayTag(ApplyTag); break;
				}
				break;
			}
		case EAddOrRemove::Remove:AbilitySystemComponent->RemoveLooseGameplayTag(ApplyTag); break;
		}
	}
	
}
