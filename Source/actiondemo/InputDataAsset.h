// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputFold/InputInfo.h"
#include "InputAction.h"
#include "InputDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	FGameplayTag InputTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	UInputAction * InputAction;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	bool CanbeHold;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	bool bAbilityInput;
	//只有当是技能时才能打开设置
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input",meta=(EditCondition="bAbilityInput"))
	EInputWeightType InputType;
};
UCLASS()
class ACTIONDEMO_API UInputDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputSetting",meta=(TitleProperty="InputTag"))
	TArray<FInputData>NativeInputData;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputSetting",meta=(TitleProperty="InputTag"))
	TArray<FInputData>AbilityInputData;

	UFUNCTION()
	UInputAction * GetNativeInputActionBytag(FGameplayTag InputTag);
	UFUNCTION()
	TArray<FInputData>  GetNativeInputActions();
	UFUNCTION()
	UInputAction * GetAbilityInputActionBytag(FGameplayTag Inputtag);
	UFUNCTION()
	TArray<FInputData>  GetAbilityInputActions();
};
