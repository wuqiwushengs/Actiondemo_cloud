// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
class FEasyPoseStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static const ISlateStyle& Get();
	static FName GetStyleSetName();
private:
	static TSharedRef< class FSlateStyleSet > Create();
private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};