// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "actiondemo/InputDataAsset.h"
#include "Act_InputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONDEMO_API UAct_InputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAct_InputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	template<typename Userobject,typename CallBackFunction>
	void BindAbilityFunctions( TArray<FInputData>& InputData,Userobject * owner,CallBackFunction FunctionName) ;
};
//绑定技能函数
template <typename Userobject, typename CallBackFunction>
inline void UAct_InputComponent::BindAbilityFunctions( TArray<FInputData>& InputData, Userobject* owner,CallBackFunction FunctionName)
{
	for (FInputData & Data : InputData)
	{
	BindAction(Data.InputAction,ETriggerEvent::Started,owner,FunctionName,Data.InputTag);
	if (Data.CanbeHold)
	{
		BindAction(Data.InputAction,ETriggerEvent::Triggered,owner,FunctionName,Data.InputTag);
		BindAction(Data.InputAction,ETriggerEvent::Completed,owner,FunctionName,Data.InputTag);
	}
	}

}


