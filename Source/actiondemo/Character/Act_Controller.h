// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "actiondemo/Act_TagContainer.h"
#include "EnhancedInputSubsystems.h"
#include "actiondemo/InputDataAsset.h"
#include "Act_Controller.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONDEMO_API AAct_Controller : public APlayerController
{
	GENERATED_BODY()
public:
	AAct_Controller();
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="InputSetting")
	TObjectPtr<UInputDataAsset> InputDataAsset;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="InputSetting")
	UInputMappingContext * NativeInputMappingContext;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="InputSetting")
	UInputMappingContext * AbilityInputMappingContext;
	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	UInputDataAsset * GetInputDataAsset();
	
};
