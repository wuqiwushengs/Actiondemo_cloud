// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Act_AbilityTypes.h"
#include "Engine/DataTable.h"
#include "actiondemo/Act_TagContainer.h"
#include "Act_AbilityDatas.generated.h"

/**
 * 用来添加获取的各种
 */
UCLASS()
class ACTIONDEMO_API UAct_AbilityDatas : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UAct_AbilityDatas();
	//仅限放入Act_AbilityTypes的内容
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="AbilitiesContent")
	UDataTable * AbilitiesContent;
	//分开的轻攻击和重攻击
	UPROPERTY()
	TArray<FAct_AbilityTypes> AbilitTypesRelaxHead;
	UPROPERTY()
	TArray<FAct_AbilityTypes> AbilitTypesHeavyHead;
	//通过此来区分轻攻击和重攻击
	UFUNCTION()
	bool SortAbilityTypesByAttackType(const FAct_AbilityTypes &A,const FAct_AbilityTypes &B);
};
