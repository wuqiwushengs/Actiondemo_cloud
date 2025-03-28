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
	//仅限放入Act_AbilityTypes的内容，要注意每个位置只能放置一个x或Y不能出现 xx xx重复出现的情况
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="AbilitiesContent")
	UDataTable * AbilitiesContent;
	
	
};

UCLASS()
class ACTIONDEMO_API UAct_AbilityDatasManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="AbilitiesContent")
	TObjectPtr<UAct_AbilityDatas> AbilityData;
	UPROPERTY()
	TArray<FAct_AbilityTypes> AbilitTypesRelaxHead;
	UPROPERTY()
	TArray<FAct_AbilityTypes> AbilitTypesHeavyHead;
	UFUNCTION()
	virtual void init();
};

