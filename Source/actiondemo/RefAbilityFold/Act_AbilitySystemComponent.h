// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "actiondemo/InputDataAsset.h"
#include "Act_AbilitySystemComponent.generated.h"


struct FInputActionInstance;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONDEMO_API UAct_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAct_AbilitySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="InputDeal")
	void ProcessingInputData(const FInputActionInstance& ActionInstance,FGameplayTag Inputag,const UInputDataAsset * InputDataAsset);
};
