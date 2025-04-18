#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterState:uint8
{
	UnAttacking,
	Attacking,
};

UENUM(BlueprintType)
enum class ECharacterUnAttackingState:uint8
{
	Normal,
	Defense,
	Rolling,
};
UENUM(BlueprintType)
enum class  EEigthDirectionState:uint8
{
	Forward,
	Backward,
	Left,
	Right,
	ForwardLeft,
	ForwardRight,
	BackwardLeft,
	BackwardRight,
	None,
};
