// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Controller.h"

#include "Act_Character.h"
#include "actiondemo/InputFold/Act_InputComponent.h"


AAct_Controller::AAct_Controller()
{
	
}

void AAct_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UAct_InputComponent* EnhancedInputComponent = CastChecked<UAct_InputComponent>(InputComponent);
	UEnhancedInputLocalPlayerSubsystem * EnhancedInputSubSystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer());
	checkf(EnhancedInputComponent,TEXT("Can't Find EnhancedInputComponent"));
	checkf(EnhancedInputSubSystem,TEXT("Can't Find EnhancedInputSubSystem"));
	AAct_Character *RefPlayer=CastChecked<AAct_Character>(InPawn);
	//授予角色输入数据：
	RefPlayer->InputDataAsset=GetInputDataAsset();
	checkf(RefPlayer->InputDataAsset,TEXT("Can't ADDInputData"));
	//
	checkf(Player,TEXT("Can't Find Player"));
	if (EnhancedInputSubSystem)
	{	EnhancedInputSubSystem->AddMappingContext(NativeInputMappingContext,1);
		UInputAction*  MoveAction=InputDataAsset->GetNativeInputActionBytag(ActTagContainer::CharacterMoveAround);
		UInputAction*  LookAroundActon=InputDataAsset->GetNativeInputActionBytag(ActTagContainer::CharacterCameraMoveAround);
		check(MoveAction);
		check(LookAroundActon);
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,RefPlayer,&AAct_Character::MoveAround);
		EnhancedInputComponent->BindAction(LookAroundActon,ETriggerEvent::Triggered,RefPlayer,&AAct_Character::LookAround);
		if (InputDataAsset->AbilityInputData.Num()>0)
		{	EnhancedInputSubSystem->AddMappingContext(AbilityInputMappingContext,0);
			EnhancedInputComponent->BindAbilityFunctions(InputDataAsset->AbilityInputData,RefPlayer,&AAct_Character::BindSkill);
		}
	}
}

UInputDataAsset* AAct_Controller::GetInputDataAsset()
{
	return InputDataAsset;
}
