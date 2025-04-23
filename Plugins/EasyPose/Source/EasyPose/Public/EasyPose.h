// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "EPRetargeter.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
class FToolBarBuilder;
class FMenuBuilder;
DEFINE_LOG_CATEGORY_STATIC(EasyPose, Log, All);
class FEasyPoseModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void PluginButtonClicked();
	void OnPostEngineInit();
	static uint8 sShowWarning;
private:
	int MatchPose(class UIKRetargeter *iKRetargeter);
private:
	TSharedPtr<class FUICommandList> PluginCommands;
	uint32 MatchingFlag = static_cast<uint32>(Matching::EFlag::All);
	MatchMode MatchingMode = MatchMode::DYNAMIC;
};
