#include "CameraRotateNotify.h"

#include "KismetAnimationLibrary.h"
#include "actiondemo/Character/Act_Character.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCameraRotateNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AAct_Character* Character = Cast<AAct_Character>(MeshComp->GetOwner());
	if(!Character) return;
	float alphavalue=Character->Enemy?0.05:0.2;
	FRotator Rotator=UKismetMathLibrary::RLerp(Character->GetControlRotation(),Character->GetActorRotation(),alphavalue,true);
	UE_LOG(LogTemp,Warning,TEXT("Rotate"));
	Character->GetController()->SetControlRotation(Rotator);
}


