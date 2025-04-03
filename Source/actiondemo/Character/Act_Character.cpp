// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"



// Sets default values
AAct_Character::AAct_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActAbilitySystemComponent=CreateDefaultSubobject<UAct_AbilitySystemComponent>("Act_AbilitySystemComponent");
	SpringArmComponent=CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	SpringArmComponent->SetupAttachment(RootComponent);
	bUseControllerRotationYaw=false;
	GetCharacterMovement()->bUseControllerDesiredRotation=true;
}

// Called when the game starts or when spawned
void AAct_Character::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AAct_Character::Tick(float DeltaTime)
{	Super::Tick(DeltaTime);
	if (GetAct_AbilitySystemComponent()->InputTagsInbuff.Num()>0)
	{
		for (int i=0;i<GetAct_AbilitySystemComponent()->InputTagsInbuff.Num();i++)
		{
			GEngine->AddOnScreenDebugMessage(-1,0.1,FColor::Black,FString::Printf(TEXT("%s"),*GetAct_AbilitySystemComponent()->InputTagsInbuff[i].InputTag.ToString()));
		}
	}
	
}

UAbilitySystemComponent* AAct_Character::GetAbilitySystemComponent() const 
{
	return  Cast<UAbilitySystemComponent>(ActAbilitySystemComponent);
}

UAct_AbilitySystemComponent* AAct_Character::GetAct_AbilitySystemComponent() const
{
	return ActAbilitySystemComponent;
}


// Called to bind functionality to input
void AAct_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAct_Character::MoveAround(const FInputActionValue& InputAction)
{
	FVector2d InputValue=InputAction.Get<FVector2d>();
	FRotator ControllRotation=GetControlRotation();
	FVector ForwardVector=UKismetMathLibrary::GetForwardVector(ControllRotation);
	ForwardVector.Normalize();
	FVector RightVector=UKismetMathLibrary::GetRightVector(ControllRotation);
	RightVector.Normalize();
	AddMovementInput(ForwardVector,InputValue.Y);
	AddMovementInput(RightVector,InputValue.X);
}

void AAct_Character::LookAround(const FInputActionValue& InputAction)
{
	FVector2d InputValue=InputAction.Get<FVector2d>();
	AddControllerPitchInput(InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void AAct_Character::LockSystem(const FInputActionValue& InputAction)
{
	if (!GetAct_AbilitySystemComponent()->GetOwnedGameplayTags().HasTagExact(ActTagContainer::LockTarget))
	{
		GetAct_AbilitySystemComponent()->AddLooseGameplayTag(ActTagContainer::LockTarget);
	}
	else
	{
		GetAct_AbilitySystemComponent()->RemoveLooseGameplayTag(ActTagContainer::LockTarget);
	}
	
}


void AAct_Character::BindSkill(const FInputActionInstance& ActionInstance, FGameplayTag Inputag)
{	
	if (InputDataAsset)
	{
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Started)
		{
			GetAct_AbilitySystemComponent()->ProcessingInputDataStarted(ActionInstance,Inputag,InputDataAsset);
		}
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Triggered)
		{
			GetAct_AbilitySystemComponent()->ProcessingInputDataTriggering(ActionInstance,Inputag,InputDataAsset);
		}
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Completed)
		{
			GetAct_AbilitySystemComponent()->ProcessingInputDataComplete(ActionInstance,Inputag,InputDataAsset);
		}
	}
	
}

