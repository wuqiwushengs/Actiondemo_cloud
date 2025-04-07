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
	TMap<ECharacterUnAttackingState,UDataTable*>AbilitiesContent;
	//各种其他类型非连招的Act_AbilityTypes的内容
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="AbilitiesContent")
	UDataTable* AbilitiesNotInComboChain;
	//TODO::写读取内容的函数
	UFUNCTION()
	bool GetAbilityTypesNotInComboChainByTag(FGameplayTag InputTag,FAct_AbilityTypes &Ability);
};

USTRUCT()
struct FAct_Abilities
{
	GENERATED_BODY()
	TArray<FAct_AbilityTypes> AbilityTypes;
	
};

USTRUCT()
struct FAbilitySum
{
	GENERATED_BODY()
	UPROPERTY()
	TMap<ECharacterUnAttackingState,FAct_Abilities> AbilityTypesRelaxHead;
	UPROPERTY()
	TMap<ECharacterUnAttackingState,FAct_Abilities> AbilityTypesHeavyHead;
};
UCLASS()
class ACTIONDEMO_API UAct_AbilityDatasManager : public UObject
{
	GENERATED_BODY()

public:
	//存储着多个状态的数据,通过读取不同状态的datatable来获取
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="AbilitiesContent")
	TObjectPtr<UAct_AbilityDatas> AbilityData;
	//分着多个状态的技能数据
	UPROPERTY()
	FAbilitySum AbilitySum;
	UFUNCTION()
	virtual void init();
	void ClearAbilityData();
	TMap<ECharacterUnAttackingState,FAct_Abilities> LoadAbilityData();
	FAbilitySum LoadAbilitySum(TMap<ECharacterUnAttackingState,FAct_Abilities> Ability);
	
};

