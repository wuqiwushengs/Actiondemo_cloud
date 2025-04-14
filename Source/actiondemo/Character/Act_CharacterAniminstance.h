// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Act_CharacterAniminstance.generated.h"

class UAbilitySystemComponent;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ACTIONDEMO_API UAct_CharacterAniminstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	/*,meta=(BlueprintThreadSafe)*/
	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe))
	UCharacterMovementComponent * GetCharacterMovementComponent () const;
	UFUNCTION(BlueprintCallable,meta=(BlueprintThreadSafe))
	FGameplayTagContainer GetAbilitySystemComponentTag() const;
	UFUNCTION(meta=(BlueprintThreadSafe))
	void SetVelocityData(float DeltaSeconds);
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	FVector CurrentVelocity;
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	FVector LastVelocity;
	FVector CurrentActorLocation;
	FVector LastActorLocation;
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	float  ActorMovementDistance;
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	FVector ActorMovementDirection;
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	float ActorMovementAngle;
	UPROPERTY(BlueprintReadOnly,Category="VelocityData")
	bool bMoved;
};
