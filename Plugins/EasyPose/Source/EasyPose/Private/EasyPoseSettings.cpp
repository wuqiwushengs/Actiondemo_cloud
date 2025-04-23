// Copyright longlt00502@gmail.com 2023. All rights reserved.
#include "EasyPoseSettings.h"
UEasyPoseSettings::UEasyPoseSettings(const FObjectInitializer& ObjectInitlaizer)
	: Super(ObjectInitlaizer)
{}
FName UEasyPoseSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
#if WITH_EDITOR
FText UEasyPoseSettings::GetSectionText() const
{
	return NSLOCTEXT("EasyPosePlugin", "EasyPoseSettingsSection", "Easy Pose Settings");
}
#endif
