// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputAction.h"
#include "actiondemo/Act_TagContainer.h"
#include "actiondemo/InputDataAsset.h"
#include "actiondemo/RefAbilityFold/Act_AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Act_Character.generated.h"

class UCameraComponent;
class USpringArmComponent;

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
UCLASS()
class ACTIONDEMO_API AAct_Character : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAct_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//Ability Interface start
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()  const override;
	//AbilityInterface End
	FORCEINLINE UAct_AbilitySystemComponent * GetAct_AbilitySystemComponent()  const  ;
	
	//CameraInit(temp)
	//@TODO:CreateCameraSystem;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<UCameraComponent>CameraComponent;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//InputBind
#pragma region InputBindFunction
	void MoveAround(const FInputActionValue& InputAction);
	void LookAround(const FInputActionValue& InputAction);
	void LockSystem(const FInputActionValue& InputAction);
	//@TODO:MakeInputDetect
	void BindSkill(const FInputActionInstance& ActionInstance,FGameplayTag Inputag);
#pragma endregion InputBindFunction

	//Property
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Abilitysystem")
	TObjectPtr<UAct_AbilitySystemComponent> ActAbilitySystemComponent;
	

	UPROPERTY(BlueprintReadWrite,Category="InputDatat")
	TObjectPtr<UInputDataAsset> InputDataAsset;
};
