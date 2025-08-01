﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "actiondemo/Character/Act_CharacterBase.h"
#include "actiondemo/RefAbilityFold/AttributeContent/Act_AttributeSet.h"
#include "Act_EnemyBase.generated.h"

UCLASS()
class ACTIONDEMO_API Act_EnemyBase : public Act_CharacterBase,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Act_EnemyBase();
	//被攻击到的结果
	FHitResult AttackResult;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UPROPERTY()
	UAbilitySystemComponent *AbilitySystemComponent;
	//AttributeSet
	UPROPERTY()
	const UAct_AttributeSet * CombatAttribute;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
