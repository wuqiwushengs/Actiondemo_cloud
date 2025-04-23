// Copyright longlt00502@gmail.com 2023. All rights reserved.
#include "EasyPose.h"
#include "EasyPoseStyle.h"
#include "EasyPoseCommands.h"
#include "Misc/MessageDialog.h"
#include "Toolkits/ToolkitManager.h"
#include "Algo/Reverse.h"
#include "RetargetEditor/IKRetargetEditor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Input/STextComboBox.h"
#include "SWarningOrErrorBox.h"
#include "EPRetargeter.h"
#include "EPUtils.h"
#define LOCTEXT_NAMESPACE "FEasyPoseModule"
uint8 FEasyPoseModule::sShowWarning = 0;
void FEasyPoseModule::StartupModule()
{
    FEasyPoseStyle::Initialize();
    FEasyPoseStyle::ReloadTextures();
    FEasyPoseCommands::Register();
    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FEasyPoseCommands::Get().PluginAction,
        FExecuteAction::CreateRaw(this, &FEasyPoseModule::PluginButtonClicked),
        FCanExecuteAction());
    FCoreDelegates::OnPostEngineInit.AddRaw(this, &FEasyPoseModule::OnPostEngineInit);
}
void FEasyPoseModule::OnPostEngineInit()
{
    if (GEditor != nullptr)
    {
        auto AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        AssetEditorSubsystem->OnAssetEditorOpened().AddLambda([this](UObject *Obj)
                                                              {
            TSharedPtr<FIKRetargetEditor> IKRetargetEditor = EPUtils::GetAssetEditorInstance<FIKRetargetEditor, UIKRetargeter>(Cast<UIKRetargeter>(Obj));
            if(!IKRetargetEditor.IsValid()) return;
            TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
            IKRetargetEditor->AddToolbarExtender(ToolbarExtender);
            ToolbarExtender->AddToolBarExtension(
                "Asset",
                EExtensionHook::Before, PluginCommands,
                FToolBarExtensionDelegate::CreateLambda([this, Obj](FToolBarBuilder& ToolbarBuilder) {
                    ToolbarBuilder.BeginSection("Easy Pose");
                    ToolbarBuilder.AddWidget(SNew(SImage)
                        .DesiredSizeOverride(FVector2D(32,32))
                        .Image_Lambda([this]() { return FEasyPoseModule::sShowWarning > 1 ? FAppStyle::Get().GetBrush("Icons.ErrorWithColor") : FAppStyle::Get().GetBrush("Icons.WarningWithColor"); })
                        .ColorAndOpacity(FSlateColor::UseForeground())
                        .Visibility_Lambda([]()
                        {
                            return FEasyPoseModule::sShowWarning > 0 ? EVisibility::Visible : EVisibility::Collapsed;
                        })
                    );
                    ToolbarBuilder.AddToolBarButton(
                        FExecuteAction::CreateLambda([this, Obj] {
                            MatchPose(Cast<UIKRetargeter>(Obj));
                        }),
                        NAME_None, 
                        TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]() { return FText::FromString("Easy Pose"); })),
                        TAttribute<FText>(), 
                        TAttribute<FSlateIcon>::Create(TAttribute<FSlateIcon>::FGetter::CreateLambda([this]() { return FSlateIcon("EditorStyle", "GraphEditor.CustomEvent_16x"); }))
                    );
                    ToolbarBuilder.AddComboButton(
                        FUIAction(),
                        FOnGetContent::CreateLambda([this]() {
                            FMenuBuilder MenuBuilder(true, PluginCommands);
                            MenuBuilder.BeginSection(NAME_None, TAttribute<FText>(FText::FromString("Matching Steps")));
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "Subsequence Chain"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingFlag ^= static_cast<uint32>(Matching::EFlag::SubChain); }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return (MatchingFlag & static_cast<uint32>(Matching::EFlag::SubChain)) != 0; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "Multi Subsequence Chain"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingFlag ^= static_cast<uint32>(Matching::EFlag::MultiSubChain); }),
                                    FCanExecuteAction::CreateLambda([this]() { return (MatchingFlag & static_cast<uint32>(Matching::EFlag::SubChain)) != 0; }),
                                    FIsActionChecked::CreateLambda([this]() { return (MatchingFlag & static_cast<uint32>(Matching::EFlag::MultiSubChain)) != 0; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "Leaf Bone"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingFlag ^= static_cast<uint32>(Matching::EFlag::LeafBone); }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return (MatchingFlag & static_cast<uint32>(Matching::EFlag::LeafBone)) != 0; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
                            MenuBuilder.EndSection();
#if (defined EP_DEVELOPMENT)
                            MenuBuilder.BeginSection(NAME_None, TAttribute<FText>(FText::FromString("Floating Point Tolerance")));
                            MenuBuilder.AddMenuEntry(FUIAction(),
                                SNew(STextComboBox)
                                .ToolTipText(LOCTEXT("", ""))
                                .OptionsSource(&FPToleranceOptions)
                                .OnSelectionChanged_Lambda([this](TSharedPtr<FString> InString, ESelectInfo::Type SelectInfo)
                                {
                                    if(InString == FPToleranceOptions[0]) {
                                        FPTolerance = 1e-2;
                                        RotationOffsetFloatingPointNegativeExp = 2;
                                    } else if(InString == FPToleranceOptions[1]) {
                                        FPTolerance = 1e-4;
                                        RotationOffsetFloatingPointNegativeExp = 4;
                                    } else {
                                        FPTolerance = 1e-8;
                                        RotationOffsetFloatingPointNegativeExp = 8;
                                    }
                                    FPTSelected = InString;
                                })
                                .InitiallySelectedItem(FPTSelected),
                                NAME_None, TAttribute<FText>(), EUserInterfaceActionType::None, NAME_None
                            );
                            MenuBuilder.EndSection();
                            MenuBuilder.BeginSection(NAME_None, TAttribute<FText>(FText::FromString("Matching Mode")));
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "Dynamic"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingMode = static_cast<MatchMode>(static_cast<uint8>(MatchingMode) ^ 1); }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return MatchingMode == MatchMode::DYNAMIC; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
                            MenuBuilder.EndSection();
                            MenuBuilder.BeginSection(NAME_None, TAttribute<FText>(FText::FromString("Development")));
                            MenuBuilder.AddMenuEntry(FUIAction(),
                                SNew(SNumericEntryBox<float>)
                                    .Value_Lambda([this](){ return (BoneAxesDotProductScalar + 1) / 2; })
                                    .AllowSpin(true)
                                    .MinValue(0).MaxValue(1.0f)
                                    .MinSliderValue(0).MaxSliderValue(1.0f)
                                    .WheelStep(0.01f)
                                    .OnValueChanged_Lambda([this](float NewValue){
                                        BoneAxesDotProductScalar = FMath::Clamp(NewValue, 0, 1.f) * 2 - 1;
                                    }),
                                NAME_None, TAttribute<FText>(), EUserInterfaceActionType::None, NAME_None
                            );
                            MenuBuilder.AddMenuEntry(FUIAction(),
                                SNew(SNumericEntryBox<int32>)
                                    .Value_Lambda([this](){ return RotationOffsetFloatingPointNegativeExp; })
                                    .AllowSpin(true)
                                    .MinValue(1).MaxValue(8).MinSliderValue(1).MaxSliderValue(8)
                                    .WheelStep(1)
                                    .OnValueChanged_Lambda([this](int32 NewValue){
                                        RotationOffsetFloatingPointNegativeExp = FMath::Clamp(NewValue, 1, 8);
                                        FPTolerance = FMath::Pow(10.0f, RotationOffsetFloatingPointNegativeExp * (-1));
                                    }),
                                NAME_None, TAttribute<FText>(), EUserInterfaceActionType::None, NAME_None
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("ESPMATitle", "Dynamic"),
                                LOCTEXT("ESPMATooltip", "Mix between Similar and Exact"),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingMode = MatchMode::DYNAMIC; }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return MatchingMode == MatchMode::DYNAMIC; })
                                ), NAME_None,
                                EUserInterfaceActionType::RadioButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("ESPMETitle", "Exact"),
                                LOCTEXT("ESPMETooltip", "Always matching Exact"),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingMode = MatchMode::EXACT; }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return MatchingMode == MatchMode::EXACT; })
                                ), NAME_None,
                                EUserInterfaceActionType::RadioButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("ESPMSTitle", "Similar"),
                                LOCTEXT("ESPMSTooltip", "Always matching Similar"),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { MatchingMode = MatchMode::SIMILAR; }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return MatchingMode == MatchMode::SIMILAR; })
                                ), NAME_None,
                                EUserInterfaceActionType::RadioButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "Always Reset"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { AlwaysReset = !AlwaysReset; }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return AlwaysReset; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
                            MenuBuilder.AddMenuEntry(
                                LOCTEXT("", "StepMiddleBoneXOnly"),
                                TAttribute<FText>(),
                                FSlateIcon(),
                                FUIAction(
                                    FExecuteAction::CreateLambda([this]() { StepMiddleBoneXOnly = !StepMiddleBoneXOnly; }),
                                    FCanExecuteAction(),
                                    FIsActionChecked::CreateLambda([this]() { return StepMiddleBoneXOnly; })
                                ), NAME_None,
                                EUserInterfaceActionType::ToggleButton
                            );
#endif
                            return MenuBuilder.MakeWidget();
                        }),
                        LOCTEXT("EPS_Label", ""),
                        LOCTEXT("EPS_ToolTip", ""),
                        FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Recompile"),
                        true);
                    ToolbarBuilder.AddSeparator();
                    ToolbarBuilder.EndSection();
                })
            );
            IKRetargetEditor->RegenerateMenusAndToolbars(); });
    }
}
void FEasyPoseModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FEasyPoseStyle::Shutdown();
    FEasyPoseCommands::Unregister();
}
int32 FEasyPoseModule::MatchPose(UIKRetargeter *IKRetargeter)
{
    TSharedPtr<FIKRetargetEditor> IKRetargetEditor = EPUtils::GetAssetEditorInstance<FIKRetargetEditor, UIKRetargeter>(IKRetargeter);
    if (!IKRetargetEditor.IsValid()) return -1;
    UIKRetargeterController *IKRtgCtrl = UIKRetargeterController::GetController(IKRetargeter);
    TSharedRef<FIKRetargetEditorController> IKRtgEditCtrl = IKRetargetEditor->GetController();
    if (IKRtgCtrl == nullptr) return -1;
    auto IsEditingTarget = IKRtgEditCtrl->GetSourceOrTarget() == ERetargetSourceOrTarget::Target;
    auto Target = IsEditingTarget? ERetargetSourceOrTarget::Target : ERetargetSourceOrTarget::Source;
    FEasyPoseModule::sShowWarning = 0;
    FEPRetargeter EPRetargeter(IKRetargeter);
    EPRetargeter.Init(GetMutableDefault<UEasyPoseSettings>());
    EPRetargeter.Dump();
    int32 ChangedCount = EPRetargeter.MatchPose(IKRetargeter, Target, MatchingFlag);
    if(ChangedCount > -1)
    {
        UE_LOG(EasyPose, Log, TEXT("Asset \"%s\": made %d adjustments with pose: \"%s\"."),
               *IKRetargeter->GetName(), ChangedCount,
               *IKRtgCtrl->GetCurrentRetargetPoseName(Target).ToString());
    }
    else
    {
        UE_LOG(EasyPose, Warning, TEXT("Failed to match pose of \"%s\"."), *IKRetargeter->GetName());
    }
    return ChangedCount;
}
void FEasyPoseModule::PluginButtonClicked()
{
    auto ToolMenus = UToolMenus::Get();
    TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
    FEditorModeTools &EditorModeTools = GLevelEditorModeTools();
    UAssetEditorSubsystem *AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    if (AssetEditorSubsystem)
    {
        TArray<UObject *> OpenAssetEditors = AssetEditorSubsystem->GetAllEditedAssets();
        for (auto RtgAsset : OpenAssetEditors)
        {
            MatchPose(Cast<UIKRetargeter>(RtgAsset));
        }
    }
}
#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FEasyPoseModule, EasyPose)