// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Controller.h"
#include "Act_Character.h"
#include "actiondemo/InputFold/Act_InputComponent.h"



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
		UInputAction* LockAction=InputDataAsset->GetNativeInputActionBytag(ActTagContainer::LockTarget);
		UInputAction* RestController=InputDataAsset->GetNativeInputActionBytag(ActTagContainer::InputRestController);
		check(MoveAction);
		check(LookAroundActon);
		check(LockAction);
		check(RestController);
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,RefPlayer,&AAct_Character::MoveAround);
		EnhancedInputComponent->BindAction(LookAroundActon,ETriggerEvent::Completed,RefPlayer,&AAct_Character::OnlookAroundEnd);
		EnhancedInputComponent->BindAction(LookAroundActon,ETriggerEvent::Triggered,RefPlayer,&AAct_Character::LookAround);
		EnhancedInputComponent->BindAction(LockAction,ETriggerEvent::Started,RefPlayer,&AAct_Character::LockSystem);
		EnhancedInputComponent->BindAction(RestController,ETriggerEvent::Started,RefPlayer,&AAct_Character::ResetController);
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
