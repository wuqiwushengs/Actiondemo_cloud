// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterInferface.h"
#include "InputAction.h"
#include "actiondemo/Act_TagContainer.h"
#include "actiondemo/InputDataAsset.h"
#include "CharacterTypes.h"
#include "actiondemo/RefAbilityFold/Act_AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Act_Character.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ACTIONDEMO_API AAct_Character : public ACharacter,public IAbilitySystemInterface,public ICharacterInferface
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
	//Character Interface Start
	virtual ECharacterUnAttackingState GetCharacterUnAttackingState_Implementation() override;
	virtual UInputDataAsset* GetCharacterInputData_Implementation() override;
	virtual void SetCharacterAttackingState_Implementation(ECharacterState State) override;
	virtual void SetCharacterUnAttackingState_Implementation(ECharacterUnAttackingState State) override;
	//Character Interface End
	//CameraInit(temp)
	//@TODO:CreateCameraSystem;
	
#pragma region CameraSystem
	//CameraSystem
	//当遇到强敌时，通过系统自动锁定敌人，不允许角色自由转换视角。
	UPROPERTY()
	bool CanMovAroundFree=true;
	UFUNCTION()
	void OnlookAroundEnd(const FInputActionValue& InputAction);
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<UCameraComponent>CameraComponent;
	float PitchSum;
	bool bForward=false;
	FVector2D InputValue;
	bool LockOnce=false;
	UFUNCTION()
	void CheckMovemntInfo();
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> CameraCurve;
	//当长时间朝着同一个方向移动时自动将摄像机转向角色背后。
	UPROPERTY(EditDefaultsOnly)
	float AutoTurnThresholdTime=1.5;
	UPROPERTY()
	float CurrentHoldTime=0;
	bool bStartTurn=false;
	UPROPERTY(DisplayName="LongtimeMovementSaveDirection")
	EEigthDirectionState LastDirectionState=EEigthDirectionState::None;
	UPROPERTY(DisplayName="LongtimeMovementSaveDirection")
	EEigthDirectionState CurrentDirectionState=EEigthDirectionState::None;
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="LongtimeMovementSaveDirection")
	EEigthDirectionState  CalculateMovementDirection();
	UFUNCTION()
	bool TurnToController();
#pragma endregion CameraSystem
	
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

//Playerstate
#pragma region PlayerState
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Playerstate")
	ECharacterState CharacterState=ECharacterState::UnAttacking;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Playerstate")
	ECharacterUnAttackingState CharacterUnAttackingState=ECharacterUnAttackingState::Normal;
#pragma endregion PlayerState
};
