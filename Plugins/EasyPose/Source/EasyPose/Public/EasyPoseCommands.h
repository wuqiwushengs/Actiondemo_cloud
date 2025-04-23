// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EasyPoseStyle.h"
class FEasyPoseCommands : public TCommands<FEasyPoseCommands>
{
public:
	FEasyPoseCommands()
		: TCommands<FEasyPoseCommands>(TEXT("EasyPose"), NSLOCTEXT("Contexts", "EasyPose", "EasyPose Plugin"), NAME_None, FEasyPoseStyle::GetStyleSetName())
	{
	}
	virtual void RegisterCommands() override;
public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
