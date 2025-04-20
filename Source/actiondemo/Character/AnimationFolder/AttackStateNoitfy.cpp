#include "AttackStateNoitfy.h"
#include "actiondemo/Character/Act_Character.h"

FLinearColor UAttackStateNoitfy::GetEditorColor()
{
	return FLinearColor::Gray;
}

void UAttackStateNoitfy::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	UAnimNotify::Notify(MeshComp, Animation, EventReference);
	AAct_Character * Character=Cast<AAct_Character>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetCharacterAttackingState_Implementation(ECharacterState::Attacking);
	}
}
	
