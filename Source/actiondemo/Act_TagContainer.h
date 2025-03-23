﻿#pragma once
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ActTagContainer
{	//Native
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CharacterMoveAround);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CharacterCameraMoveAround);
	//Ability Input
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityTest);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputRelaxAttack);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputHeavyAttack);
	//锁定因为牵涉到摄像机而非人物的技能，因此实际上是在native部分进行绑定
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LockTarget);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputDefense);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputRolling);
	//Ability 这里是每个招式的AbilityTag;
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RelaxAttack);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HeavyAttack);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Defense);
	ACTIONDEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Rolling);
}

