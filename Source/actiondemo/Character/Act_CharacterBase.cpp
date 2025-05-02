// Fill out your copyright notice in the Description page of Project Settings.
#include "Act_CharacterBase.h"
#include "actiondemo/RefAbilityFold/Act_AbilitySystemComponent.h"



// Sets default values
Act_CharacterBase::Act_CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
// Called when the game starts or when spawned
void Act_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Act_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void Act_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

