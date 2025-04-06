#pragma once
#include "Act_Ability.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "actiondemo/Character/CharacterTypes.h"
#include "Act_AbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct FAct_AbilityTypes: public FTableRowBase
{
	GENERATED_BODY()
public:
	FAct_AbilityTypes(){};
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAct_Ability> Ability;
	UPROPERTY(EditDefaultsOnly)
	ECharacterUnAttackingState AttackingState=ECharacterUnAttackingState::Normal;
	UPROPERTY()
	bool CanbeHold;
	//对应的输入tag(Relax OR heavy)
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
	//技能表 为固定的字符在Act_AbilitySystemComponent中的宏编写
	UPROPERTY(EditDefaultsOnly)
	FString AbilityList;
	//这个技能使用时需要有什么tag比如在地上的时候 用前x和原地x的攻击不同
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer OwnerRequiresTag;
	//获取技能的长度
	int32 GetAbilityListSize();
	//获取技能的内容通过索引
	TCHAR * GetAbilityListContentByIndex(int32 index=0,bool GetEnd=true);
	
	
	
};
