// Copyright longlt00502@gmail.com 2023. All rights reserved.
#include "EasyPoseCommands.h"
#define LOCTEXT_NAMESPACE "FEasyPoseModule"
void FEasyPoseCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EasyPose", "Pose Now", EUserInterfaceActionType::Button, FInputChord());
}
#undef LOCTEXT_NAMESPACE
