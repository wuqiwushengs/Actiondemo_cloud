// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterInferface.generated.h"

enum class ECharacterUnAttackingState : uint8;
// This class does not need to be modified.
UINTERFACE()
class UCharacterInferface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONDEMO_API ICharacterInferface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	ECharacterUnAttackingState GetCharacterUnAttackingState();
	virtual ECharacterUnAttackingState GetCharacterUnAttackingState_Implementation()=0;
};
