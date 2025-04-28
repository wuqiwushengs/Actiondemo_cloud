// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Function.h"

EEigthDirectionState UAct_Function::CalculateMovementsDirection(float angle)
{
	float absAngle=FMath::Abs(angle);
	if (absAngle<=22.5)
	{
		return  EEigthDirectionState::Forward;
	}
	if (absAngle>=155.5)
	{
		return EEigthDirectionState::Backward;
	}
	if ((absAngle-90)<=22.5)
	{
		if (angle<0) return EEigthDirectionState::Left;
		return EEigthDirectionState::Right;
	}
	if ((absAngle-45)<=22.5)
	{
		if (angle<0) return EEigthDirectionState::ForwardLeft;
		return EEigthDirectionState::ForwardRight;
	}
	if ((absAngle-135)<=22.5)
	{
		if (angle<0) return EEigthDirectionState::BackwardLeft;
		return EEigthDirectionState::BackwardRight;
	}
	return EEigthDirectionState::None;
}

float UAct_Function::GetOwnedWalkSpeed(AAct_Character* Character)
{
	float FinalSpeed=0;
	switch (Character->CharacterState)
	{
		case ECharacterState::UnAttacking: FinalSpeed=500.0f; break;
		case ECharacterState::Attacking:FinalSpeed=0; break;
	}
	switch (Character->CharacterUnAttackingState)
	{
		case ECharacterUnAttackingState::Normal: FinalSpeed=500.0f; break;
		case ECharacterUnAttackingState::Defense: FinalSpeed=0; break;
		case ECharacterUnAttackingState::Rolling: FinalSpeed=0; break;
	}

	return FinalSpeed;
}
