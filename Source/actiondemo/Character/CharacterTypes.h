#pragma once
#include "CoreMinimal.h"
#include "CharacterTypes.generated.h"

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
USTRUCT(BlueprintType)
struct FSpringArmValue
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	float ArmLength;
	UPROPERTY(EditDefaultsOnly)
	FVector slotoffset;
	
};

