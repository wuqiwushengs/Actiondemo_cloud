#include "Act_TagContainer.h"

namespace ActTagContainer
{
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(CharacterMoveAround,"NativeInput.MoveAround");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(CharacterCameraMoveAround,"NativeInput.CameraMoveAround")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(AbilityTest,"Ability.AbilityTest")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(InputRelaxAttack,"AttackStatement.Attack.Input.RelaxAttack")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(InputHeavyAttack,"AttackStatement.Attack.Input.HeavyAttack")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(LockTarget,"AttackStatement.LockTarget")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(InputDefense,"AttackStatement.Attack.Input.Defense")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(InputRolling,"AttackStatement.Attack.Input.Rolling")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(RelaxAttack,"AttackStatement.Attack.Ability.RelaxAttack")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(HeavyAttack,"AttackStatement.Attack.Ability.HeavyAttack")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(Defense,"AttackStatement.Attack.Ability.Defense")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(Rolling,"AttackStatement.Attack.Ability.Rolling")
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(Pressed,"Input.Pressed");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(Released,"Input.Released");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(ExePreInputRelaxAttack,"Input.Manage.ExePreInputRelaxAttack");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(ExeMulityInputRelaxAttack,"Input.Manage.ExeMutualityInputRelaxAttack");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(ExeInterrupt,"Character.State.InterruptClass");
	ACTIONDEMO_API UE_DEFINE_GAMEPLAY_TAG(ExeMulityInputExeAttack,"Input.Manage.ExeMutualityInputExeAttack");
	
}