// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_AbilitySystemComponent.h"


// Sets default values for this component's properties
UAct_AbilitySystemComponent::UAct_AbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAct_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAct_AbilitySystemComponent::ProcessingInputData(const FInputActionInstance& ActionInstance, FGameplayTag Inputag,const UInputDataAsset* InputDataAsset)
{
}

