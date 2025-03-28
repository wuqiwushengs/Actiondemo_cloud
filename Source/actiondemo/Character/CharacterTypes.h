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