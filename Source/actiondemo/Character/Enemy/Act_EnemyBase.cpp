// Fill out your copyright notice in the Description page of Project Settings.

#include "Act_EnemyBase.h"

#include "actiondemo/RefAbilityFold/AttributeContent/Act_AttributeSet.h"


// Sets default values
Act_EnemyBase::Act_EnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	CombatAttribute=CreateDefaultSubobject<UAct_AttributeSet>("AttributeSet");
}

// Called when the game starts or when spawned
void Act_EnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UAbilitySystemComponent* Act_EnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called every frame
void Act_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void Act_EnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

