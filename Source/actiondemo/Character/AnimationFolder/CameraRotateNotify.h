#pragma once
#include"CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CameraRotateNotify.Generated.h"

UCLASS()
class ACTIONDEMO_API UCameraRotateNotify:public  UAnimNotify
{
public:
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
