// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EasyPoseSettings.generated.h"
UENUM(meta = (Bitflags, Hidden))
enum class EAlongAxesFlag : uint8
{
    X   ,
    Y   ,
    Z   ,
};
ENUM_CLASS_FLAGS(EAlongAxesFlag)
UENUM()
enum class EAlongAxes : uint8
{
    X           = 1u << static_cast<uint8>(EAlongAxesFlag::X),
    Y           = 1u << static_cast<uint8>(EAlongAxesFlag::Y),
    Z           = 1u << static_cast<uint8>(EAlongAxesFlag::Z),
    All             = (X | Y | Z)
};
USTRUCT()
struct FAlignBoneAlongAxes
{
    GENERATED_BODY()
public:
    UPROPERTY(config, EditAnywhere, Category = EasyPose)
    FName BoneName;
    UPROPERTY(config, EditAnywhere, Category = EasyPose, meta = (Bitmask, BitmaskEnum = "/Script/EasyPose.EAlongAxesFlag"))
    uint8 AlongAxes = StaticCast<uint8>(EAlongAxes::All);
    UPROPERTY(config, EditAnywhere, Category = EasyPose)
    bool ApplyChildren = true;
};
UCLASS(config=EasyPose, defaultconfig)
class EASYPOSE_API UEasyPoseSettings: public UDeveloperSettings
{
    GENERATED_UCLASS_BODY()
public:
    UPROPERTY(config, EditAnywhere, Category = EasyPose, meta = (AllowPrivateAccess = "true"))
    TArray<FName> IgnoreTargetChain;
    UPROPERTY(config, EditAnywhere, Category = EasyPose, meta = (AllowPrivateAccess = "true"))
    TArray<FAlignBoneAlongAxes> AlignTargetBoneAlongAxes;
    virtual FName GetCategoryName() const override;
#if WITH_EDITOR
    virtual FText GetSectionText() const override;
#endif
};
