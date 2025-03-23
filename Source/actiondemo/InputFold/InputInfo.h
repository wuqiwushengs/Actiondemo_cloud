#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

UENUM()
enum class EInputWeightType:uint8
{
	//@TODO::处理在按到相同权重的按键处理过程
	//各种技能的权重一定要是完全不同的，这里可以向后添加，但一定不要相同（有时间这个问题会处理）
	Weight0=0,
	Weight1=1,
	Weight2=2,
	Weight3=3,
	Weight4=4,
	Weight5=5,
	Weight6=6,
	Weight7=7,
};

