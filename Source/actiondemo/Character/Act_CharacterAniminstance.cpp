// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_CharacterAniminstance.h"

#include "Act_Character.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAct_CharacterAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UAct_CharacterAniminstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UAct_CharacterAniminstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	SetVelocityData(DeltaSeconds);
}

UCharacterMovementComponent* UAct_CharacterAniminstance::GetCharacterMovementComponent() const
{	USkeletalMeshComponent * SkelMeshComponentS=GetSkelMeshComponent();
	ACharacter *Character=Cast<ACharacter>(SkelMeshComponentS->GetOwner());
	if (Character)
	{	UCharacterMovementComponent * CharacterMovementComponent=Character->GetCharacterMovement();
		return  CharacterMovementComponent;
	}
	return  nullptr;
}

FGameplayTagContainer UAct_CharacterAniminstance::GetAbilitySystemComponentTag() const
{	AAct_Character*character= Cast<AAct_Character>(TryGetPawnOwner());
	if (character)
	{
		UAbilitySystemComponent *AbilitySystemComponent=character->GetAbilitySystemComponent();
		if(AbilitySystemComponent)
		{
			return  AbilitySystemComponent->GetOwnedGameplayTags();
		}
	}
	return FGameplayTagContainer();
}

void UAct_CharacterAniminstance::SetVelocityData(float DeltaSeconds)
{
	LastVelocity=CurrentVelocity;
	LastActorLocation=CurrentActorLocation;
	if (GetCharacterMovementComponent())
	{
		CurrentVelocity=GetCharacterMovementComponent()->GetLastUpdateVelocity();
	}
	if (TryGetPawnOwner())
	{
		FRotator ActorRotatoion=TryGetPawnOwner()->GetActorRotation();
		ActorMovementAngle=UKismetAnimationLibrary::CalculateDirection(CurrentVelocity,ActorRotatoion);
	}
	if (TryGetPawnOwner())
	{
		CurrentActorLocation=TryGetPawnOwner()->GetActorLocation();
	}
	ActorMovementDistance=FVector::Distance(CurrentActorLocation,LastActorLocation);
	FMath::Abs(ActorMovementDistance)>0.0f?bMoved=true:bMoved=false;
	ActorMovementDirection=(CurrentActorLocation-LastActorLocation).GetSafeNormal();

	
	
}
