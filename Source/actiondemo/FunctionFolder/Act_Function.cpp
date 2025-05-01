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

UAnimMontage* UAct_Function::GetCorrectHitMontageByAngle(const FHitResult& HitResult,
	TMap<EMontageDirection, UAnimMontage*> MontageSource)
{	//获取打击的actor
	ACharacter* HitActor=Cast<ACharacter>(HitResult.GetActor());
	if(HitActor)
	{	//获取空间世界发现
		FVector WorldNormal=HitResult.ImpactNormal;
		//转换到actor本地空间
		FVector LocalNormal=HitActor->GetActorTransform().InverseTransformVectorNoScale(WorldNormal);
		LocalNormal.Normalize();
		//计算水平投影
		FVector HorizontalNormal=FVector(LocalNormal.X,LocalNormal.Y,0).GetSafeNormal();
		//计算角度
		float AngleRadians=FMath::Atan2(HorizontalNormal.Y,HorizontalNormal.X);
		float AngleDegrees=FMath::RadiansToDegrees(AngleRadians);
		if(FMath::Abs(AngleDegrees)<=30.0f)
		{
			if (UAnimMontage**ForwardMontage=MontageSource.Find(EMontageDirection::Forward))
			{
				return *ForwardMontage;
			}
			return nullptr;
		}
		if (FMath::Abs(AngleDegrees)>=55.0f)
		{
			if (UAnimMontage **BackMontage=MontageSource.Find(EMontageDirection::Back))
			{
				return  * BackMontage;
				
			}
			return nullptr;
		}
		if (AngleDegrees>0)
		{
			if (UAnimMontage ** RightMontage=MontageSource.Find(EMontageDirection::Right))
			{
				return  *RightMontage;
			}
			return nullptr;
		}
		if (AngleDegrees<0)
		{
			if (UAnimMontage ** LeftMontage=MontageSource.Find(EMontageDirection::Left))
			{
				return *LeftMontage;
			}
			return nullptr;
		}
	}
	return nullptr;
}

