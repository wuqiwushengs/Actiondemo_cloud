// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Character.h"


// Sets default values
AAct_Character::AAct_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActAbilitySystemComponent=CreateDefaultSubobject<UAct_AbilitySystemComponent>("Act_AbilitySystemComponent");
}

// Called when the game starts or when spawned
void AAct_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAct_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveAround");
}

void AAct_Character::LookAround(const FInputActionValue& InputAction)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "LookAround");
}

void AAct_Character::BindSkill(const FInputActionValue& InputAction, FGameplayTag Inputag)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BindSkill"));

}

