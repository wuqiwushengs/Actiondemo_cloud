// Fill out your copyright notice in the Description page of Project Settings.


#include "InputDataAsset.h"

UInputAction* UInputDataAsset::GetNativeInputActionBytag(FGameplayTag InputTag)
{
	UInputAction* NativeInputAction=nullptr;
	if (InputTag.IsValid())
	{
		for (FInputData & Data: NativeInputData)
		{
			if (Data.InputTag==InputTag)
			{
				NativeInputAction=Data.InputAction ;
				return  NativeInputAction;
			}
		}
	}
	if (NativeInputAction==nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Can't Find Native InputAction"))
	}
	return NativeInputAction; 
}

TArray<FInputData> UInputDataAsset::GetNativeInputActions()
{
	return NativeInputData;
}

UInputAction* UInputDataAsset::GetAbilityInputActionBytag(FGameplayTag Inputtag)
{
	UInputAction* AbilityInputAction=nullptr;
	if (Inputtag.IsValid())
	{
		for (FInputData & Data: NativeInputData)
		{
			if (Data.InputTag==Inputtag) 
			{
				AbilityInputAction=Data.InputAction ;
				return  AbilityInputAction;
			}
		}
	}
	if (AbilityInputAction==nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Can't Find Native InputAction"))
	}
	return AbilityInputAction; 
}

TArray<FInputData> UInputDataAsset::GetAbilityInputActions()
{
	return  AbilityInputData;
}
