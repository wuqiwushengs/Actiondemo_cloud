// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_AddorRemoveTag.generated.h"


enum class EAddOrRemove : uint8;
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAN_AddorRemoveTag : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ExposeOnSpawn))
	EAddOrRemove ApplyTagState;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ExposeOnSpawn))
	FGameplayTag ApplyTag;
	
};
