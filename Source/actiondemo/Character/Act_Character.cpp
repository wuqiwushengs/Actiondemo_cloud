﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Character.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "KismetAnimationLibrary.h"
#include "Camera/CameraComponent.h"
#include  "actiondemo/FunctionFolder/Act_Function.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "actiondemo/Character/Enemy/Act_EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "actiondemo/RefAbilityFold/AttributeContent/Act_AttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AAct_Character::AAct_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComponent=CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	SpringArmComponent->SetupAttachment(RootComponent);
	bUseControllerRotationYaw=false;
	GetCharacterMovement()->bUseControllerDesiredRotation=true;
	CombatAttribute=CreateDefaultSubobject<UAct_AttributeSet>("Attribute");
}

// Called when the game starts or when spawned
void AAct_Character::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AAct_Character::Tick(float DeltaTime)
{	Super::Tick(DeltaTime);
	//视角处理的内容：
	if (!Enemy)
	{
		CheckMovemntInfo();
		checkCameraCollision();
	}
	if (Enemy)
	{
		FollowingEnemy();
	}
	
}

UAbilitySystemComponent* AAct_Character::GetAbilitySystemComponent() const 
{
	return  Cast<UAbilitySystemComponent>(ActAbilitySystemComponent);
}
UAct_AbilitySystemComponent* AAct_Character::GetAct_AbilitySystemComponent() const
{
	return ActAbilitySystemComponent;
}
ECharacterUnAttackingState AAct_Character::GetCharacterUnAttackingState_Implementation()
{
	return CharacterUnAttackingState;
	
}
UInputDataAsset* AAct_Character::GetCharacterInputData_Implementation()
{
	return InputDataAsset;
}
void AAct_Character::SetCharacterAttackingState_Implementation(ECharacterState State)
{
	CharacterState=State;
	//当切换为不攻击时则更改为普通状态
	if (State==ECharacterState::UnAttacking)
	{
		GetAct_AbilitySystemComponent()->SetInputstate(InputState::NormalInputState);
	}
	GetCharacterMovement()->MaxWalkSpeed=UAct_Function::GetOwnedWalkSpeed(this);
}
void AAct_Character::SetCharacterUnAttackingState_Implementation(ECharacterUnAttackingState State)
{
	CharacterUnAttackingState=State;
	GetCharacterMovement()->MaxWalkSpeed=UAct_Function::GetOwnedWalkSpeed(this);
	
	
}
void AAct_Character::CheckMovemntInfo()
{
#pragma  region NormalMovement
	FVector Velocity=GetCharacterMovement()->GetLastUpdateVelocity();
	if (CharacterState==ECharacterState::Attacking)
	{	
		return ;
	}
	if (!Velocity.IsZero()&&!bStartTurn&&!GetMesh()->IsPlayingRootMotion())
	{
		
		LastDirectionState=CurrentDirectionState;
		CurrentDirectionState=CalculateMovementDirection();
		//如果当前方向和上次方向相同并且没有开始旋转，则增加计时
		if (CurrentDirectionState==LastDirectionState)
		{
				CurrentHoldTime+=GetWorld()->GetDeltaSeconds();
				if (CurrentHoldTime>=AutoTurnThresholdTime)
				{
					bStartTurn=true;
					CurrentHoldTime=0;
				}
		}
		else 
		{
			CurrentHoldTime=0;
		}
		if (!bStartTurn)
		{
			//Calculate CameraRotation with the character's movement
			float angle=UKismetAnimationLibrary::CalculateDirection(Velocity,GetControlRotation());
			float value=CameraCurve->GetFloatValue(angle);
			AddControllerYawInput(value*2);
		}
	}
	if (bStartTurn)
	{
	   bStartTurn= !TurnToController();
	}
}
#pragma endregion NormalMovement
#pragma region LongtimeMovementSaveDirection
EEigthDirectionState AAct_Character::CalculateMovementDirection()
{
	float angle=UKismetAnimationLibrary::CalculateDirection(GetVelocity(),FRotator(0,1,0));
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

bool AAct_Character::TurnToController()
{
	if (FMath::Abs(CameraComponent->GetComponentRotation().Yaw-GetActorRotation().Yaw)<10)
	{	 SpringArmComponent->CameraRotationLagSpeed=5;
		return true;
	}	//当转换视角之后，因为控制器的旋转导致角色的移动方向和输入值不一致,会导致方向不同。需要直接更改输入值为向前，然后等待这个数值变更时再进行变化。
		GetLocalViewingPlayerController()->SetControlRotation(GetActorRotation());
	    SpringArmComponent->CameraRotationLagSpeed=10;
		bForward=true;
		if (FMath::Abs(CameraComponent->GetComponentRotation().Yaw-GetActorRotation().Yaw)<10)
		{  SpringArmComponent->CameraRotationLagSpeed=5;
			return true;
		}
	    return false;
}

void AAct_Character::checkCameraCollision()
{
	if (PlayerControlLookaxis)return;
	TArray<AActor*> IgnoreActor;
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMulti(this,CameraComponent->GetComponentLocation(),
		CameraComponent->GetComponentLocation()+GetControlRotation().Vector(),30.0f,ETraceTypeQuery::TraceTypeQuery3,
		false,IgnoreActor,EDrawDebugTrace::None,HitResults,false);
	for (FHitResult & Result:HitResults)
	{
		if (!Result.bBlockingHit||bStartTurn) return;
		bool HitCharacter=Result.GetComponent()->ComponentHasTag(FName("Player"));
		if (HitCharacter)
		{
			if (!GetCharacterMovement()->Velocity.IsZero())
			{
				bStartTurn=true;
				return;
			}
			GetLocalViewingPlayerController()->SetControlRotation(GetActorRotation());
			
			
		}
	}
}

void AAct_Character::FollowingEnemy()
{
	FVector EnemyLocation=Enemy->GetActorLocation();
	FRotator Rotation= UKismetMathLibrary::FindLookAtRotation(CameraComponent->GetComponentLocation(),EnemyLocation);
	GetLocalViewingPlayerController()->SetControlRotation(Rotation);
}

void AAct_Character::MeetBoss(AActor* Boss)
{
	Enemy=Boss;
	CanMovAroundFree=false;
	SpringArmComponent->TargetArmLength=SpringArmDefaultValue[1].ArmLength;
	SpringArmComponent->SocketOffset=SpringArmDefaultValue[1].slotoffset;
}

void AAct_Character::BossLeave()
{
	Enemy=nullptr;
	CanMovAroundFree=true;
	SpringArmComponent->TargetArmLength=SpringArmDefaultValue[0].ArmLength;
	SpringArmComponent->SocketOffset=SpringArmDefaultValue[0].slotoffset;
}
#pragma endregion LongtimeMovementSaveDirection
// Called to bind functionality to input
void AAct_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAct_Character::MoveAround(const FInputActionValue& InputAction)
{
	if (GetAct_AbilitySystemComponent()->GetOwnedGameplayTags().HasTagExact(ActTagContainer::WeakState)) return;
	if (!bForward)
	{
		InputValue=InputAction.Get<FVector2D>();
		FRotator ControllRotation=GetControlRotation();
		FVector ForwardVector=UKismetMathLibrary::GetForwardVector(ControllRotation);
		ForwardVector.Normalize();
		FVector RightVector=UKismetMathLibrary::GetRightVector(ControllRotation);
		RightVector.Normalize();
		AddMovementInput(ForwardVector,InputValue.Y);
		AddMovementInput(RightVector,InputValue.X);
		
	}
	else
	{
		if (!LockOnce)
		{
			InputValue=InputAction.Get<FVector2D>();
			LockOnce=true;
		}
		FRotator ControllRotation=GetControlRotation();
		float ValueX=FMath::Abs(InputValue.X-InputAction.Get<FVector2D>().X);
		float ValueY=FMath::Abs(InputValue.Y-InputAction.Get<FVector2D>().Y);
		if (ValueX>0.2||ValueY>0.2||GetCharacterMovement()->GetLastUpdateVelocity().Length()<=10)
		{
			bForward=false;
			LockOnce=false;
		}
		FVector ForwardVector=UKismetMathLibrary::GetForwardVector(ControllRotation);
		AddMovementInput(ForwardVector,1);
	}
}
void AAct_Character::LookAround(const FInputActionValue& InputAction)
{
	if (!CanMovAroundFree)return;
	PlayerControlLookaxis=true;
	FVector2D InputValues=InputAction.Get<FVector2D>();
	//限制相机的角度，并且让每次转动只允许一个角度，同时当y轴没有角度旋转时或者结束旋转时都归零。
	if (FMath::Abs(InputValues.X)>FMath::Abs(InputValues.Y))
	{
	AddControllerYawInput(InputValues.X*0.3);
	PitchSum=0;
	GetLocalViewingPlayerController()->SetControlRotation(FRotator(0,GetControlRotation().Yaw,0));
	}
	else if ((PitchSum+InputValues.Y*0.3)<=20&&(PitchSum+InputValues.Y*0.3)>=-20)
	{	PitchSum+=InputValues.Y*0.3;
	AddControllerPitchInput(InputValues.Y*0.3);
	}
}

void AAct_Character::LockSystem(const FInputActionValue& InputAction)
{
	if (!GetAct_AbilitySystemComponent()->GetOwnedGameplayTags().HasTagExact(ActTagContainer::LockTarget))
	{
		GetAct_AbilitySystemComponent()->AddLooseGameplayTag(ActTagContainer::LockTarget);
	}
	else
	{
		GetAct_AbilitySystemComponent()->RemoveLooseGameplayTag(ActTagContainer::LockTarget);
	}
	
}

void AAct_Character::ResetController(const FInputActionValue& InputAction)
{
	if (!Enemy)
	{
		bStartTurn=true;
		bForward=true;
	}
	
}
void AAct_Character::BindSkill(const FInputActionInstance& ActionInstance, FGameplayTag Inputag)
{	//如果是虚弱状态那么所有操作都只会走向从虚弱转向普通状态
	if (GetAbilitySystemComponent()->GetOwnedGameplayTags().HasTag(ActTagContainer::WeakState))
	{
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Started&&CanMoveAfterWeak)
		{
			FGameplayTagContainer WeakToNormal(ActTagContainer::Weak2Normal);
			GetAct_AbilitySystemComponent()->TriggerTag=FGameplayTag();
			if (GetAct_AbilitySystemComponent()->TryActivateAbilitiesByTag(WeakToNormal))
			{
				CanMoveAfterWeak=false;
			}
		}
		
	}
	else
	{
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Started)
		{
			GetAct_AbilitySystemComponent()->ProcessingInputDataStarted(ActionInstance,Inputag,InputDataAsset);
		}
		
		
		if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Ongoing)
		{	
			GetAct_AbilitySystemComponent()->ProcessingInputDataTrigger(ActionInstance,Inputag,InputDataAsset,ActionInstance.GetElapsedTime());
		}
	}
	if (ActionInstance.GetTriggerEvent()==ETriggerEvent::Completed)
	{
		GetAct_AbilitySystemComponent()->ProcessingInputDataComplete(ActionInstance,Inputag,InputDataAsset);
	}	
}

void AAct_Character::CheckRollingCanExecuteAndExecute(const FInputActionValue& InputAction)
{	//在防御状态固定设置朝向
	if (GetAct_AbilitySystemComponent()->GetOwnedGameplayTags().HasTagExact(ActTagContainer::WeakState)) return;
	if (GetAct_AbilitySystemComponent()->GetOwnedGameplayTags().HasTag(ActTagContainer::InputDefense))
	{
		DashDirection2d=InputAction.Get<FVector2D>();
		FGameplayTagContainer DashContainer;
		DashContainer.AddTag(ActTagContainer::Rolling);
		if (GetAct_AbilitySystemComponent()->TryActivateAbilitiesByTag(DashContainer))
		{
			GetAct_AbilitySystemComponent()->SetInputstate(InputState::DisableInputState);
		}
	
	}
	
}

void AAct_Character::TryAttackTrace(bool blineTrace)
{
	if (blineTrace)
	{
		for (TPair<FName,FLineTraceInfo> & BoneInfo:TraceBoneAndInfo)
		{	BoneInfo.Value.LastLocation=BoneInfo.Value.CurrenLocation;
			BoneInfo.Value.CurrenLocation=GetMesh()->GetSocketLocation(BoneInfo.Key);
			if (BoneInfo.Value.CurrenLocation!=FVector::ZeroVector&&BoneInfo.Value.LastLocation!=FVector::ZeroVector)
			{
				FHitResult  HitResult;
				TArray<AActor*> IgnoreActor;
				IgnoreActor.Add(this);
				UKismetSystemLibrary::SphereTraceSingle(this,BoneInfo.Value.LastLocation,BoneInfo.Value.CurrenLocation,25,TraceTypeQuery1,false,IgnoreActor,EDrawDebugTrace::ForOneFrame,HitResult,true);
			
				if (HitResult.IsValidBlockingHit()&&HitResult.GetActor())
				{
					Act_EnemyBase * Character=Cast<Act_EnemyBase>(HitResult.GetActor());
					if (Character&&!AttackedActor.Contains(Character))
					{
						AttackedActor.Add(Character);
						FGameplayEffectContextHandle AttackContent=ActAbilitySystemComponent->MakeEffectContext();
						Character->AttackResult=HitResult;
						AttackContent.AddInstigator(this,this);
						AttackContent.AddHitResult(HitResult);
						AttackContent.AddSourceObject(this);
						FGameplayEffectSpecHandle AvailableEffectHandle=ActAbilitySystemComponent->MakeOutgoingSpec(AttackEffect,1.f,AttackContent);
						//增加伤害效果
						FGameplayEffectContextHandle AffectContextHandle=ActAbilitySystemComponent->MakeEffectContext();
						AffectContextHandle.AddSourceObject(this);
						FGameplayEffectSpecHandle DamageHandle=ActAbilitySystemComponent->MakeOutgoingSpec(AddDamageEffect,1.f,AffectContextHandle);
						DamageHandle.Data->SetSetByCallerMagnitude(ActTagContainer::DamageValue,AttackValue);
						//提交效果
						GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*DamageHandle.Data.Get());
						GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*AvailableEffectHandle.Data.Get(),Character->GetAbilitySystemComponent());
						check(HitSound);
						UGameplayStatics::PlaySoundAtLocation(this,HitSound,HitResult.Location);
						FGameplayTagContainer VfxTag{ActTagContainer::AttackTimeDilamation};
						GetAct_AbilitySystemComponent()->TryActivateAbilitiesByTag(VfxTag);
					}
				}
			}
		}
	}
else
{
	TPair<FName,FLineTraceInfo>& FootBoneAndInfo=*SingleBoneAndInfo.begin();
	FootBoneAndInfo.Value.LastLocation=FootBoneAndInfo.Value.CurrenLocation;
	FootBoneAndInfo.Value.CurrenLocation=GetMesh()->GetSocketLocation(FootBoneAndInfo.Key);
	
	FHitResult  HitResult;
	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(this);
	if (FootBoneAndInfo.Value.CurrenLocation!=FVector::ZeroVector&&FootBoneAndInfo.Value.LastLocation!=FVector::ZeroVector)
	{
		UKismetSystemLibrary::SphereTraceSingle(this,FootBoneAndInfo.Value.LastLocation,FootBoneAndInfo.Value.CurrenLocation,25,TraceTypeQuery1,false,IgnoreActor,EDrawDebugTrace::Persistent,HitResult,true);
	}
	if (HitResult.IsValidBlockingHit()&&HitResult.GetActor())
	{
		Act_EnemyBase * Character=Cast<Act_EnemyBase>(HitResult.GetActor());
		if (Character&&!AttackedActor.Contains(Character))
		{
			AttackedActor.Add(Character);
			FGameplayEffectContextHandle AttackContent=ActAbilitySystemComponent->MakeEffectContext();
			Character->AttackResult=HitResult;
			AttackContent.AddInstigator(this,this);
			AttackContent.AddHitResult(HitResult);
			AttackContent.AddSourceObject(this);
			FGameplayEffectSpecHandle AvailableEffectHandle=ActAbilitySystemComponent->MakeOutgoingSpec(AttackEffect,1.f,AttackContent);
			//给自身攻击力的
			FGameplayEffectContextHandle AffectContextHandle=ActAbilitySystemComponent->MakeEffectContext();
			AffectContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle DamageHandle=ActAbilitySystemComponent->MakeOutgoingSpec(AddDamageEffect,1.f,AffectContextHandle);
			DamageHandle.Data->SetSetByCallerMagnitude(ActTagContainer::DamageValue,AttackValue);
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*DamageHandle.Data.Get());
				GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*AvailableEffectHandle.Data.Get(),Character->GetAbilitySystemComponent());
				check(HitSound);
				UGameplayStatics::PlaySoundAtLocation(this,HitSound,HitResult.Location);
			FGameplayTagContainer VfxTag{ActTagContainer::AttackTimeDilamation};
			GetAct_AbilitySystemComponent()->TryActivateAbilitiesByTag(VfxTag);
		}
					 
	}
}
}
#pragma region CameraSystem
void AAct_Character::OnlookAroundEnd(const FInputActionValue& InputAction)
{
	PitchSum=0;
	PlayerControlLookaxis=false;
	GetLocalViewingPlayerController()->SetControlRotation(FRotator(0,GetControlRotation().Yaw,0));
}
#pragma endregion CameraSystem
