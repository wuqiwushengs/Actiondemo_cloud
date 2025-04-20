#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackStateNoitfy.generated.h"

UCLASS()
class ACTIONDEMO_API UAttackStateNoitfy:public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual FLinearColor GetEditorColor() override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
