// Copyright longlt00502@gmail.com 2023. All rights reserved.
#include "EPRetargeter.h"
#include "Misc/ScopedSlowTask.h"
#include "RetargetEditor/IKRetargetEditor.h"
#include "EasyPose.h"
#include "EasyPoseSettings.h"
#include "EPUtils.h"
#define RETARGETSKELETON_INVALID_BRANCH_INDEX -2
#define DELTA_TOLERANCE UE_SMALL_NUMBER
void FEPRetargetSkeleton::Initialize(
	USkeletalMesh* InSkeletalMesh, FName const &RetargetRootBoneName)
{
	Reset();
	SkeletalMesh = InSkeletalMesh;
	const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetRefSkeleton();
	BoneNames.Init(NAME_None, RefSkeleton.GetNum());
	ParentIndices.Init(INDEX_NONE, RefSkeleton.GetNum());
	for (int32 BoneIndex=0; BoneIndex<RefSkeleton.GetNum(); ++BoneIndex)
	{
		BoneNames[BoneIndex] = RefSkeleton.GetBoneName(BoneIndex);
		ParentIndices[BoneIndex] = RefSkeleton.GetParentIndex(BoneIndex);
	}
	CachedEndOfBranchIndices.Init(RETARGETSKELETON_INVALID_BRANCH_INDEX, ParentIndices.Num());
	RetargetLocalPose = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
	RetargetGlobalPose = RetargetLocalPose;
	UpdateGlobalTransformsBelowBone(0);
	ChainThatContainsBone.Init(NAME_None, BoneNames.Num());
	RetargetRootBoneIndex = FindBoneIndexByName(RetargetRootBoneName);
}
void FEPRetargetSkeleton::GenerateRetargetPose(const FIKRetargetPose* InRetargetPose)
{
	if(RetargetRootBoneIndex == INDEX_NONE)
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Lack of retarget root."));
		return;
	}
	RetargetLocalPose = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
	RetargetGlobalPose = RetargetLocalPose;
	UpdateGlobalTransformsBelowBone(0);
	const TArray<FTransform>& RefPoseLocal = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
	FTransform& RootTransform = RetargetGlobalPose[RetargetRootBoneIndex];
	RootTransform.AddToTranslation(InRetargetPose->GetRootTranslationDelta());
	UpdateLocalTransformOfSingleBone(RetargetRootBoneIndex, RetargetLocalPose, RetargetGlobalPose);
	for (const TTuple<FName, FQuat>& BoneDelta : InRetargetPose->GetAllDeltaRotations())
	{
		const int32 BoneIndex = FindBoneIndexByName(BoneDelta.Key);
		if (BoneIndex == INDEX_NONE)
		{
			continue;
		}
		const FQuat LocalBoneRotation = RefPoseLocal[BoneIndex].GetRotation() * BoneDelta.Value;
		RetargetLocalPose[BoneIndex].SetRotation(LocalBoneRotation.GetNormalized());
	}
	UpdateGlobalTransformsBelowBone(0);
}
TArray<int32> FEPRetargetSkeleton::GetSortedChildrenIndicesByMostAlignedWithMedian(int32 ChainIndex) const
{
	FEPChain const &EPChain = EPChains[ChainIndex];
	FVector ParentLocation = RetargetGlobalPose[EPChain.BoneIndices.Last()].GetTranslation();
	TArray<FVector> ChildrenDir;
	FVector MedianVec = FVector::ZeroVector;
	TArray<int32> SubsequenceBoneIndices;
	TMap<int32, int32> TempMap;
	int32 Index=0;
	for(auto Child : EPChain.Children)
	{
		int32 ChildBoneIndex = Child->BoneIndices[0];
		SubsequenceBoneIndices.Add(ChildBoneIndex);
		TempMap.Add(ChildBoneIndex, Index++);
		ChildrenDir.Add((RetargetGlobalPose[ChildBoneIndex].GetTranslation() - ParentLocation).GetSafeNormal());
	}
	for(auto ChildDir : ChildrenDir)
	{
		MedianVec += ChildDir;
	}
	MedianVec.Normalize();
	TArray<float> DotProducts;
	for(int32 Id=0; Id<ChildrenDir.Num(); Id++)
	{
		DotProducts.Add(FVector::DotProduct(MedianVec, ChildrenDir[Id]));
	}
	TArray<int32> SymmetricList;
	for(int32 Id=0; Id<SubsequenceBoneIndices.Num(); Id++)
	{
		bool Found = false;
		if(SymmetricList.Contains(Id))
		{
			continue;
		}
		for(int32 Id2=Id+1; Id2<SubsequenceBoneIndices.Num(); Id2++)
		{
			if(FMath::IsNearlyEqual(DotProducts[Id], DotProducts[Id2], UE_KINDA_SMALL_NUMBER))
			{
				UE_LOG(EasyPose, VeryVerbose, TEXT("Symmetric found: %s (%f) - %s (%f)."), *BoneNames[SubsequenceBoneIndices[Id]].ToString(), DotProducts[Id], *BoneNames[SubsequenceBoneIndices[Id2]].ToString(), DotProducts[Id2]);
				SymmetricList.Add(Id2);
				Found = true;
			}
		}
		if(Found)
		{
			SymmetricList.Add(Id);
		}
	}
	SymmetricList.Sort();
	for(int32 Id=SymmetricList.Num()-1; Id>=0; Id--)
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("RemoveAt: %d/%d."), SymmetricList[Id], SubsequenceBoneIndices.Num()-1);
		SubsequenceBoneIndices.RemoveAt(SymmetricList[Id]);
	}
	SubsequenceBoneIndices.Sort([&](int32 IndexA, int32 IndexB) {
		const FVector& A = ChildrenDir[TempMap[IndexA]];
		const FVector& B = ChildrenDir[TempMap[IndexB]];
		auto DotProductA = DotProducts[TempMap[IndexA]];
		auto DotProductB = DotProducts[TempMap[IndexB]];
		UE_LOG(EasyPose, VeryVerbose, TEXT("A: %s (%f) - B: %s (%f)."), *BoneNames[IndexA].ToString(), DotProductA, *BoneNames[IndexB].ToString(), DotProductB);
		return DotProductA > DotProductB;
	});
	if(EasyPose.GetVerbosity() >= ELogVerbosity::VeryVerbose)
	{
		for(int32 Id=0; Id<SubsequenceBoneIndices.Num(); Id++)
		{
			int32 BoneIndex = SubsequenceBoneIndices[Id];
			UE_LOG(EasyPose, VeryVerbose, TEXT("- [%d] %s."), Id, *BoneNames[BoneIndex].ToString());
		}
	}
	return SubsequenceBoneIndices;
}
FEPChain *FEPRetargetSkeleton::GetEPChainParent(const FEPChain &Chain)
{
	if(Chain.BoneIndices.IsEmpty())
	{
		return nullptr;
	}
	int32 ParentBoneIndex = ParentIndices[Chain.BoneIndices[0]];
	while(ParentBoneIndex != INDEX_NONE)
	{
		FName ParentChainName = ChainThatContainsBone[ParentBoneIndex];
		if(ParentChainName != NAME_None)
		{
			int32 ParentEPChainIndex = EPChains.IndexOfByPredicate([&ParentChainName] (const FEPChain& EPChain)
			{
				return EPChain.Name == ParentChainName;
			});
			if(ParentEPChainIndex != INDEX_NONE)
			{
				return &EPChains[ParentEPChainIndex];
			}
		}
		ParentBoneIndex = ParentIndices[ParentBoneIndex];
	}
	return nullptr;
}
void FEPRetargetSkeleton::Reset()
{
	BoneNames.Reset();
	ParentIndices.Reset();
	RetargetLocalPose.Reset();
	RetargetGlobalPose.Reset();
	EPChains.Reset();
	SkeletalMesh = nullptr;
}
int32 FEPRetargetSkeleton::FindBoneIndexByName(const FName InName) const
{
	return BoneNames.IndexOfByPredicate([&InName](const FName BoneName)
	{
		return BoneName == InName;
	});
}
void FEPRetargetSkeleton::UpdateGlobalTransformsBelowBone(const int32 StartBoneIndex)
{
	UpdateGlobalTransformsBelowBone(StartBoneIndex - 1, RetargetLocalPose, RetargetGlobalPose);
}
void FEPRetargetSkeleton::UpdateGlobalTransformsBelowBone(
	const int32 StartBoneIndex,
	const TArray<FTransform>& InLocalPose,
	TArray<FTransform>& OutGlobalPose) const
{
	check(BoneNames.IsValidIndex(StartBoneIndex+1));
	check(BoneNames.Num() == InLocalPose.Num());
	check(BoneNames.Num() == OutGlobalPose.Num());
	for (int32 BoneIndex=StartBoneIndex+1; BoneIndex<OutGlobalPose.Num(); ++BoneIndex)
	{
		UpdateGlobalTransformOfSingleBone(BoneIndex,InLocalPose,OutGlobalPose);
	}
}
void FEPRetargetSkeleton::UpdateGlobalTransformOfSingleBone(
	const int32 BoneIndex,
	const TArray<FTransform>& InLocalPose,
	TArray<FTransform>& OutGlobalPose) const
{
	const int32 ParentIndex = ParentIndices[BoneIndex];
	if (ParentIndex == INDEX_NONE)
	{
		OutGlobalPose[BoneIndex] = InLocalPose[BoneIndex];
		return;
	}
	const FTransform& ChildLocalTransform = InLocalPose[BoneIndex];
	const FTransform& ParentGlobalTransform = OutGlobalPose[ParentIndex];
	OutGlobalPose[BoneIndex] = ChildLocalTransform * ParentGlobalTransform;
}
void FEPRetargetSkeleton::UpdateLocalTransformOfSingleBone(
	const int32 BoneIndex,
	TArray<FTransform>& OutLocalPose,
	const TArray<FTransform>& InGlobalPose) const
{
	const int32 ParentIndex = ParentIndices[BoneIndex];
	if (ParentIndex == INDEX_NONE)
	{
		OutLocalPose[BoneIndex] = InGlobalPose[BoneIndex];
		return;
	}
	const FTransform& ChildGlobalTransform = InGlobalPose[BoneIndex];
	const FTransform& ParentGlobalTransform = InGlobalPose[ParentIndex];
	OutLocalPose[BoneIndex] = ChildGlobalTransform.GetRelativeTransform(ParentGlobalTransform);
}
int32 FEPRetargetSkeleton::GetCachedEndOfBranchIndex(const int32 InBoneIndex) const
{
	if (!CachedEndOfBranchIndices.IsValidIndex(InBoneIndex))
	{
		return INDEX_NONE;
	}
	if (CachedEndOfBranchIndices[InBoneIndex] != RETARGETSKELETON_INVALID_BRANCH_INDEX)
	{
		return CachedEndOfBranchIndices[InBoneIndex];
	}
	const int32 NumBones = BoneNames.Num();
	if (InBoneIndex == 0)
	{
		CachedEndOfBranchIndices[InBoneIndex] = NumBones-1;
		return CachedEndOfBranchIndices[InBoneIndex];
	}
	CachedEndOfBranchIndices[InBoneIndex] = INDEX_NONE;
	const int32 StartParentIndex = GetParentIndex(InBoneIndex);
	int32 BoneIndex = InBoneIndex + 1;
	int32 ParentIndex = GetParentIndex(BoneIndex);
	while (ParentIndex > StartParentIndex && BoneIndex < NumBones)
	{
		CachedEndOfBranchIndices[InBoneIndex] = BoneIndex;
		BoneIndex++;
		ParentIndex = GetParentIndex(BoneIndex);
	}
	return CachedEndOfBranchIndices[InBoneIndex];
}
void FEPRetargetSkeleton::GetChildrenIndices(const int32 BoneIndex, TArray<int32>& OutChildren) const
{
	const int32 LastBranchIndex = GetCachedEndOfBranchIndex(BoneIndex);
	if (LastBranchIndex == INDEX_NONE)
	{
		return;
	}
	for (int32 ChildBoneIndex = BoneIndex + 1; ChildBoneIndex <= LastBranchIndex; ChildBoneIndex++)
	{
		if (GetParentIndex(ChildBoneIndex) == BoneIndex)
		{
			OutChildren.Add(ChildBoneIndex);
		}
	}
}
void FEPRetargetSkeleton::GetChildrenIndicesRecursive(const int32 BoneIndex, TArray<int32>& OutChildren) const
{
	const int32 LastBranchIndex = GetCachedEndOfBranchIndex(BoneIndex);
	if (LastBranchIndex == INDEX_NONE)
	{
		return;
	}
	for (int32 ChildBoneIndex = BoneIndex + 1; ChildBoneIndex <= LastBranchIndex; ChildBoneIndex++)
	{
		OutChildren.Add(ChildBoneIndex);
	}
}
bool FEPRetargetSkeleton::IsParentOfChild(const int32 PotentialParentIndex, const int32 ChildBoneIndex) const
{
	int32 ParentIndex = GetParentIndex(ChildBoneIndex);
	while (ParentIndex != INDEX_NONE)
	{
		if (ParentIndex == PotentialParentIndex)
		{
			return true;
		}
		ParentIndex = GetParentIndex(ParentIndex);
	}
	return false;
}
int32 FEPRetargetSkeleton::GetParentIndex(const int32 BoneIndex) const
{
	if (BoneIndex < 0 || BoneIndex>=ParentIndices.Num() || BoneIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	return ParentIndices[BoneIndex];
}
FEPResolvedBoneChain::FEPResolvedBoneChain(const FBoneChain& BoneChain, const FEPRetargetSkeleton& Skeleton, TArray<int32> &OutBoneIndices)
{
    const int32 StartIndex = Skeleton.FindBoneIndexByName(BoneChain.StartBone.BoneName);
    const int32 EndIndex = Skeleton.FindBoneIndexByName(BoneChain.EndBone.BoneName);
    bFoundStartBone = StartIndex > INDEX_NONE;
    bFoundEndBone = EndIndex > INDEX_NONE;
    const bool bIsWellFormed = bFoundStartBone && bFoundEndBone && EndIndex >= StartIndex;
    if (bIsWellFormed)
    {
        OutBoneIndices = {EndIndex};
        if (EndIndex == StartIndex)
        {
            bEndIsStartOrChildOfStart = true;
            return;
        }
        int32 ParentIndex = Skeleton.GetParentIndex(EndIndex);
        while (ParentIndex > INDEX_NONE && ParentIndex >= StartIndex)
        {
            OutBoneIndices.Add(ParentIndex);
            ParentIndex = Skeleton.GetParentIndex(ParentIndex);
        }
        if (OutBoneIndices.Last() == StartIndex)
        {
            bEndIsStartOrChildOfStart = true;
            Algo::Reverse(OutBoneIndices);
            return;
        }
        OutBoneIndices.Reset();
    }
}
FEPRetargeter::FEPRetargeter(UIKRetargeter *IKRetargeter)
{
    auto Source = ERetargetSourceOrTarget::Source;
    auto Target = ERetargetSourceOrTarget::Target;
    const UIKRigDefinition *SourceIKRig = IKRetargeter->GetIKRig(ERetargetSourceOrTarget::Source);
    const UIKRigDefinition *TargetIKRig = IKRetargeter->GetIKRig(ERetargetSourceOrTarget::Target);
    const TArray<FBoneChain> &SrcRtgChains = SourceIKRig->GetRetargetChains();
    const TArray<FBoneChain> &TgtRtgChains = TargetIKRig->GetRetargetChains();
    const FName &SrcRtgRootName = SourceIKRig->GetRetargetRoot();
    const FName &TgtRtgRootName = TargetIKRig->GetRetargetRoot();
	UIKRetargeterController *IKRtgCtrl = UIKRetargeterController::GetController(IKRetargeter);
	USkeletalMesh *SourceSkeleton = IKRtgCtrl->GetPreviewMesh(Source);
	USkeletalMesh *TargetSkeleton = IKRtgCtrl->GetPreviewMesh(Target);
    EPRetargetSkeletons[0].Initialize(SourceSkeleton, SrcRtgRootName);
    EPRetargetSkeletons[1].Initialize(TargetSkeleton, TgtRtgRootName);
	TArray<TObjectPtr<URetargetChainSettings>> const &AllChainSettings = IKRetargeter->GetAllChainSettings();
    InvalidateChainMapping(AllChainSettings, SrcRtgChains, TgtRtgChains);
}
void FEPRetargeter::Init(UEasyPoseSettings const *EPSettings)
{
	if(EPSettings == nullptr)
	{
		return;
	}
	IgnoreTargetChain = TSet<FName>(EPSettings->IgnoreTargetChain);
	for(auto const &BoneRot : EPSettings->AlignTargetBoneAlongAxes)
	{
		auto AlongAxes = BoneRot.AlongAxes;
		if(AlignTargetBoneAlongAxesMap.Contains(BoneRot.BoneName))
		{
			AlignTargetBoneAlongAxesMap[BoneRot.BoneName] = AlongAxes;
		}
		else
		{
			AlignTargetBoneAlongAxesMap.Add(BoneRot.BoneName, AlongAxes);
		}
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s: AlongAxes: 0x%X."), *BoneRot.BoneName.ToString(), AlignTargetBoneAlongAxesMap[BoneRot.BoneName]);
		if(BoneRot.ApplyChildren)
		{
			TArray<int32> Children;
			auto BoneIndex = EPRetargetSkeletons[1].FindBoneIndexByName(BoneRot.BoneName);
			EPRetargetSkeletons[1].GetChildrenIndicesRecursive(BoneIndex, Children);
			for(auto ChildIndex : Children)
			{
				auto const &BoneName = EPRetargetSkeletons[1].BoneNames[ChildIndex];
				if(AlignTargetBoneAlongAxesMap.Contains(BoneName))
				{
					AlignTargetBoneAlongAxesMap[BoneName] = AlongAxes;
				}
				else
				{
					AlignTargetBoneAlongAxesMap.Add(BoneName, AlongAxes);
				}
				UE_LOG(EasyPose, VeryVerbose, TEXT("- %s: AlongAxes: 0x%X."), *BoneName.ToString(),  AlignTargetBoneAlongAxesMap[BoneName]);
			}
		}
	}
}
TOptional<FName> FEPRetargeter::FindMappedChainWithSource(TArray<TObjectPtr<URetargetChainSettings>> const &AllChainSettings, FName const &Source) const
{
	auto MappedChain = AllChainSettings.FindByPredicate([&](TObjectPtr<URetargetChainSettings> ChainSetting)
	{
		return ChainSetting->SourceChain == Source;
	});
	if(MappedChain == nullptr)
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("No such source chain '%s' in Chain Settings."), *Source.ToString());
		return NullOpt;
	}
	return TOptional<FName>((*MappedChain)->TargetChain);
}
bool FEPRetargetSkeleton::IsRootBoneConnectToParentTail(FEPChain const &Chain) const
{
	auto const Parent = GetParentOfChain(Chain);
	if(Parent == nullptr)
	{
		return false;
	}
	if(Chain.BoneIndices.IsEmpty() || Parent->BoneIndices.IsEmpty())
	{
		return false;
	}
	auto ParentIndex = ParentIndices[Chain.BoneIndices[0]];
	while(ParentIndex != INDEX_NONE && ParentIndex > Parent->BoneIndices.Last())
	{
		ParentIndex = ParentIndices[ParentIndex];
	}
	return ParentIndex == Parent->BoneIndices.Last();
}
void FEPRetargeter::InvalidateChainMapping(TArray<TObjectPtr<URetargetChainSettings>> const &AllChainSettings, const TArray<FBoneChain> &SourceIKRigChains, const TArray<FBoneChain> &TargetIKRigChains)
{
	auto &SourceSkeleton = EPRetargetSkeletons[0];
	auto &TargetSkeleton = EPRetargetSkeletons[1];
	int32 SourceRetargetRootBoneIndex = SourceSkeleton.RetargetRootBoneIndex;
	int32 TargetRetargetRootBoneIndex = TargetSkeleton.RetargetRootBoneIndex;
	if(SourceRetargetRootBoneIndex == INDEX_NONE || TargetRetargetRootBoneIndex == INDEX_NONE)
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Lack of retarget root: %d - %d."), SourceRetargetRootBoneIndex, TargetRetargetRootBoneIndex);
	}
	for (TObjectPtr<URetargetChainSettings> const &ChainSettings : AllChainSettings)
    {
        if (ChainSettings == nullptr)
		{
			FEasyPoseModule::sShowWarning |= 2;
			UE_LOG(EasyPose, Error, TEXT("Chain is null."));
            continue;
		}
		if(ChainSettings->SourceChain == NAME_None)
		{
			UE_LOG(EasyPose, Verbose, TEXT("Ignore chain: %s"), *ChainSettings->TargetChain.ToString());
			continue;
		}
		auto TargetChain = TargetIKRigChains.FindByPredicate([ChainSettings](FBoneChain const &El)
		{
			return ChainSettings->TargetChain == El.ChainName;
		});
		auto SourceChain = SourceIKRigChains.FindByPredicate([ChainSettings](FBoneChain const &El)
		{
			return ChainSettings->SourceChain == El.ChainName;
		});
		check(TargetChain != nullptr && SourceChain != nullptr);
		auto TargetEPChain = TargetSkeleton.InvalidateBoneChain(*TargetChain);
		auto SourceEPChain = SourceSkeleton.InvalidateBoneChain(*SourceChain);
		if(!TargetEPChain.IsSet() || !SourceEPChain.IsSet())
		{
			UE_LOG(EasyPose, Verbose, TEXT("Ignore invalid chain: %s - %s."), *SourceChain->ChainName.ToString(), *TargetChain->ChainName.ToString());
			continue;
		}
		TargetSkeleton.AddEPChain(*TargetEPChain);
		SourceSkeleton.AddEPChain(*SourceEPChain);
		EPChainMappingList[0].Add(SourceEPChain->Name, TargetEPChain->Name);
		EPChainMappingList[1].Add(TargetEPChain->Name, SourceEPChain->Name);
	}
	for(int32 ChainIndex=0; ChainIndex<SourceSkeleton.EPChains.Num(); ChainIndex++)
	{
		auto &TargetChain = TargetSkeleton.EPChains[ChainIndex];
		auto &SourceChain = SourceSkeleton.EPChains[ChainIndex];
		auto TargetParentChain = TargetSkeleton.GetParentOfChain(TargetChain);
		auto SourceParentChain = SourceSkeleton.GetParentOfChain(SourceChain);
		if(TargetParentChain != nullptr && SourceParentChain != nullptr)
		{
			TargetChain.Parent = TargetParentChain;
			SourceChain.Parent = SourceParentChain;
			auto MappedTargetChain = FindMappedChainWithSource(AllChainSettings, SourceParentChain->Name);
			if(MappedTargetChain.IsSet() && *MappedTargetChain != TargetParentChain->Name)
			{
				FEasyPoseModule::sShowWarning |= 2;
				UE_LOG(EasyPose, Error, TEXT("Detected strange parent of: %s (%s) - %s (%s).")
					, *TargetChain.Name.ToString(), *TargetParentChain->Name.ToString()
					, *SourceChain.Name.ToString(), *SourceParentChain->Name.ToString()
					);
				continue;
			}
			if(!TargetSkeleton.IsRootBoneConnectToParentTail(TargetSkeleton.EPChains[ChainIndex]) || !SourceSkeleton.IsRootBoneConnectToParentTail(SourceSkeleton.EPChains[ChainIndex]))
			{
				UE_LOG(EasyPose, Verbose, TEXT("Twist chain: %s (%s) - %s (%s).")
					, *TargetChain.Name.ToString(), *TargetParentChain->Name.ToString()
					, *SourceChain.Name.ToString(), *SourceParentChain->Name.ToString()
					);
				continue;	
			}
			TargetParentChain->Children.Add(&TargetChain);
			SourceParentChain->Children.Add(&SourceChain);
		}
	}
}
void FEPChain::Dump() const
{
	if(Parent != nullptr)
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s (%s)"), *Name.ToString(), *Parent->Name.ToString());
	}
	else
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s ()"), *Name.ToString());
	}
	for(int ChildIndex=0; ChildIndex<Children.Num(); ChildIndex++)
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("- %s"), *Children[ChildIndex]->Name.ToString());
	}
}
void FEPRetargetSkeleton::Dump() const
{
	for(int32 Index=0; Index<EPChains.Num(); Index++)
	{
		EPChains[Index].Dump();
	}
}
void FEPRetargeter::Dump() const
{
	if(EasyPose.GetVerbosity() < ELogVerbosity::VeryVerbose)
	{
		return;
	}
	check(EPRetargetSkeletons[0].EPChains.Num() == EPRetargetSkeletons[1].EPChains.Num());
	for(int32 Index=0; Index<EPRetargetSkeletons[0].EPChains.Num(); Index++)
	{
		EPRetargetSkeletons[0].EPChains[Index].Dump();
		EPRetargetSkeletons[1].EPChains[Index].Dump();
	}
}
TOptional<FEPChain> FEPRetargetSkeleton::InvalidateBoneChain(FBoneChain const &BoneChain)
{
	if(EPChainMap.Contains(BoneChain.ChainName))
	{
		FEasyPoseModule::sShowWarning |= 1;
		UE_LOG(EasyPose, Warning, TEXT("Detected duplicate mapping with Source: %s."), *BoneChain.ChainName.ToString());
	}
	FEPChain EPChain;
	if (!FEPResolvedBoneChain(BoneChain, *this, EPChain.BoneIndices).IsValid())
	{
		UE_LOG(EasyPose, Verbose, TEXT("Invalid chain: %s."), *BoneChain.ChainName.ToString());
		return NullOpt;
	}
	EPChain.Name = BoneChain.ChainName;
	for (const int32 BoneIndex : EPChain.BoneIndices)
	{
		if(ChainThatContainsBone[BoneIndex] != NAME_None)
		{
			if(ChainThatContainsBone[BoneIndex] != BoneChain.ChainName)
			{
				FEasyPoseModule::sShowWarning |= 2;
				UE_LOG(EasyPose, Error, TEXT("Found %s in multiple chains: %s, and %s."), *BoneNames[BoneIndex].ToString(), *ChainThatContainsBone[BoneIndex].ToString(), *EPChain.Name.ToString());
			}
			else
			{
				continue;
			}
		}
		ChainThatContainsBone[BoneIndex] = EPChain.Name;
		if(EPChain.ContiguousBoneLengths.IsEmpty())
		{
			EPChain.ContiguousBoneLengths.Add(0);
		}
		else
		{
			EPChain.ContiguousBoneLengths.Add(EPChain.ContiguousBoneLengths.Last() + RetargetLocalPose[BoneIndex].GetTranslation().Length());
		}
	}
	return TOptional<FEPChain>(MoveTemp(EPChain));
}
int32 FEPRetargeter::MatchPose(UIKRetargeter *IKRetargeter, ERetargetSourceOrTarget RetargetDestination, uint32 InMatchFlag)
{
	UIKRetargeterController *IKRtgCtrl = UIKRetargeterController::GetController(IKRetargeter);
	if (IKRtgCtrl == nullptr) return -1;
	int32 ChangedCount = 0;
	int32 TargetId = RetargetDestination == ERetargetSourceOrTarget::Target;
	int32 SourceId = RetargetDestination == ERetargetSourceOrTarget::Source;
	auto &TargetSkeleton = EPRetargetSkeletons[TargetId];
	auto &SourceSkeleton = EPRetargetSkeletons[SourceId];
	auto const &EPChainMapping = EPChainMappingList[SourceId];
	if(SourceSkeleton.EPChains.IsEmpty() != TargetSkeleton.EPChains.IsEmpty())
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Chains are not invalidated: %d - %d."), SourceSkeleton.EPChains.Num(), TargetSkeleton.EPChains.Num());
		return ChangedCount;
	}
	if(SourceSkeleton.EPChains.Num() != TargetSkeleton.EPChains.Num())
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Number of chains between Source (%d) and Target (%d) doesn't match."), SourceSkeleton.EPChains.Num(), TargetSkeleton.EPChains.Num());
		return -1;
	}
	MatchFlag = InMatchFlag;
	EPRetargetSkeletons[0].GenerateRetargetPose(IKRetargeter->GetCurrentRetargetPose(ERetargetSourceOrTarget::Source));
	EPRetargetSkeletons[1].GenerateRetargetPose(IKRetargeter->GetCurrentRetargetPose(ERetargetSourceOrTarget::Target));
	Deltas.Init(FQuat::Identity, TargetSkeleton.BoneNames.Num());
	UE_LOG(EasyPose, Verbose, TEXT("Number of chains to be matched: %d."), TargetSkeleton.EPChains.Num());
	FScopedSlowTask MatchingPoseProgress(TargetSkeleton.EPChains.Num(), FText::FromString(TEXT("Matching poses...")));
	MatchingPoseProgress.MakeDialog();
	for(int32 ChainIndex=0; ChainIndex<TargetSkeleton.EPChains.Num(); ChainIndex++)
	{
		auto &TargetChain = TargetSkeleton.EPChains[ChainIndex];
		auto &SourceChain = SourceSkeleton.EPChains[ChainIndex];
		MatchingPoseProgress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Matching %s -> %s..."),
            *SourceChain.Name.ToString()
            , *TargetChain.Name.ToString()
            )));
		if(IgnoreTargetChain.Contains(SourceChain.Name))
		{
			UE_LOG(EasyPose, Log, TEXT("Ignore chain: %s - %s."), *TargetChain.Name.ToString(), *SourceChain.Name.ToString());
			continue;
		}
		TArray<FName> BonesToReset;
		BonesToReset.Reserve(TargetChain.BoneIndices.Num());
		for(int32 BoneIndex : TargetChain.BoneIndices)
		{
			BonesToReset.Add(TargetSkeleton.BoneNames[BoneIndex]);
			TargetSkeleton.RetargetLocalPose[BoneIndex].SetRotation(TargetSkeleton.SkeletalMesh->GetRefSkeleton().GetRefBonePose()[BoneIndex].GetRotation());
		}
		IKRtgCtrl->ResetRetargetPose(IKRetargeter->GetCurrentRetargetPoseName(RetargetDestination), BonesToReset, RetargetDestination);
		TargetSkeleton.UpdateGlobalTransformsBelowBone(TargetChain.BoneIndices[0]);
		int32 Count = MatchChain(Deltas, ChainIndex, SourceSkeleton, TargetSkeleton, MatchMode::DYNAMIC);
		if(Count < 0)
		{
			continue;
		}
		ChangedCount += Count;
		if(MatchLastBoneInChain(Deltas, ChainIndex, SourceSkeleton, TargetSkeleton, EPChainMapping))
		{
			ChangedCount++;
		}
	}
	for(int32 Id=0; Id<Deltas.Num(); Id++)
	{
		auto &Delta = Deltas[Id];
		if (!EPUtils::IsEmptyOrIdentity(Delta, DELTA_TOLERANCE))
		{
			IKRtgCtrl->SetRotationOffsetForRetargetPoseBone(TargetSkeleton.BoneNames[Id], Delta, RetargetDestination);
			UE_LOG(EasyPose, VeryVerbose, TEXT("'%s': Quaternion((%.15f, %.15f, %.15f, %.15f))"), *TargetSkeleton.BoneNames[Id].ToString(), Delta.W, Delta.X, Delta.Y, Delta.Z);
		}
	}
	return ChangedCount;
}
int32 FEPRetargeter::MatchChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton, MatchMode MatchingMode)
{
	int32 ChangedCount = 0;
	if(ChainIndex < 0 && ChainIndex >= FMath::Min(TargetSkeleton.EPChains.Num(), SourceSkeleton.EPChains.Num()))
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Invalid Chain index: %d."), ChainIndex);
		return -1;
	}
	auto TgtChain = TargetSkeleton.EPChains[ChainIndex];
	auto SrcChain = SourceSkeleton.EPChains[ChainIndex];
	if(TgtChain.BoneIndices.Num() < 2 || SrcChain.BoneIndices.Num() < 2)
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore chain has single bone: %s (%d) - %s (%d).")
			, *TgtChain.Name.ToString(), TgtChain.BoneIndices.Num()
			, *SrcChain.Name.ToString(), SrcChain.BoneIndices.Num()
			);
		return 0;
	}
	bool IsMatchExact = (MatchingMode == MatchMode::DYNAMIC) ? SrcChain.BoneIndices.Num() == TgtChain.BoneIndices.Num() : MatchingMode == MatchMode::EXACT;
	UE_LOG(EasyPose, Log, TEXT("%s: %s (%d) - %s (%d)"), IsMatchExact ? TEXT("Exact") : TEXT("Similar")
			, *TgtChain.Name.ToString(), TgtChain.BoneIndices.Num()
			, *SrcChain.Name.ToString(), SrcChain.BoneIndices.Num()
			);
	if (IsMatchExact)
	{
		auto MinLength = FMath::Min(SrcChain.BoneIndices.Num(), TgtChain.BoneIndices.Num());
		for (int32 BoneIndexId=0; BoneIndexId<MinLength-1; BoneIndexId++)
		{
			auto SrcBoneIndex = SrcChain.BoneIndices[BoneIndexId];
			auto SrcNextBoneIndex = SrcChain.BoneIndices[BoneIndexId+1];
			auto TgtBoneIndex = TgtChain.BoneIndices[BoneIndexId];
			auto TgtNextBoneIndex = TgtChain.BoneIndices[BoneIndexId+1];
			if(SrcBoneIndex == 0 || TgtBoneIndex == 0)
			{
				UE_LOG(EasyPose, Verbose, TEXT("Ignore root bone: %s (%s) - %s (%s)."), *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[0].ToString(), *SrcChain.Name.ToString(), *SourceSkeleton.BoneNames[0].ToString());
				continue;
			}
			FVector SourceDir = (SourceSkeleton.RetargetGlobalPose[SrcNextBoneIndex].GetTranslation() - SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetTranslation()).GetSafeNormal();
			if(AlignTargetBoneAlongAxesMap.Contains(TargetSkeleton.BoneNames[TgtBoneIndex]))
			{
				auto const &TargetDir = (TargetSkeleton.RetargetGlobalPose[TgtNextBoneIndex].GetTranslation() - TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetTranslation()).GetSafeNormal();
				RefineDirection(TargetSkeleton.BoneNames[TgtBoneIndex], TargetDir, SourceDir);
			}
			if(TargetSkeleton.AlignBoneWithDirection(TgtBoneIndex, TgtNextBoneIndex, SourceDir, OutDeltas))
			{
				ChangedCount++;
			}
		}
	}
	else
	{
		if(SrcChain.BoneIndices.Num() != SrcChain.ContiguousBoneLengths.Num() || TgtChain.BoneIndices.Num() != TgtChain.ContiguousBoneLengths.Num())
		{
			UE_LOG(EasyPose, Warning, TEXT("Ignore chain: %s - %s."), *TgtChain.Name.ToString(), *SrcChain.Name.ToString());
			return -1;
		}
		auto SrcWatermarkLoc = SourceSkeleton.RetargetGlobalPose[SrcChain.BoneIndices[0]].GetTranslation();
		for (int32 SrcId = 0, TgtId = 0; SrcId < SrcChain.BoneIndices.Num() - 1 && TgtId < TgtChain.BoneIndices.Num() - 1;)
        {
			auto SrcBoneIndex = SrcChain.BoneIndices[SrcId];
			auto SrcNextBoneIndex = SrcChain.BoneIndices[SrcId+1];
			auto TgtBoneIndex = TgtChain.BoneIndices[TgtId];
			auto TgtNextBoneIndex = TgtChain.BoneIndices[TgtId+1];
			float SrcTotalLenToNextPercent = SrcChain.ContiguousBoneLengths[SrcId+1] / SrcChain.ContiguousBoneLengths.Last();
			float TgtTotalLenToNextPercent = TgtChain.ContiguousBoneLengths[TgtId+1] / TgtChain.ContiguousBoneLengths.Last();
			UE_LOG(EasyPose, VeryVerbose, TEXT("Src %s: %f = %f / %f"), *SourceSkeleton.BoneNames[SrcNextBoneIndex].ToString()
					, SrcTotalLenToNextPercent, SrcChain.ContiguousBoneLengths[SrcId+1], SrcChain.ContiguousBoneLengths.Last());
			UE_LOG(EasyPose, VeryVerbose, TEXT("Tgt %s: %f = %f / %f"), *TargetSkeleton.BoneNames[TgtNextBoneIndex].ToString()
					, TgtTotalLenToNextPercent, TgtChain.ContiguousBoneLengths[TgtId+1], TgtChain.ContiguousBoneLengths.Last());
			if (TgtTotalLenToNextPercent > SrcTotalLenToNextPercent)
			{
				SrcId++;
				continue;
			}
			auto DiffLenPercentToNext = 1 - ((SrcTotalLenToNextPercent - TgtTotalLenToNextPercent) * SrcChain.ContiguousBoneLengths.Last() / (SrcChain.ContiguousBoneLengths[SrcId+1] - SrcChain.ContiguousBoneLengths[SrcId]));
			auto SrcCurrentBoneLoc = SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetTranslation();
			auto SrcNextBoneLoc = SourceSkeleton.RetargetGlobalPose[SrcNextBoneIndex].GetTranslation();
			auto NextPoint = (SrcNextBoneLoc - SrcCurrentBoneLoc) * (DiffLenPercentToNext) + SrcCurrentBoneLoc;
			auto SourceDir = (NextPoint - SrcWatermarkLoc).GetSafeNormal();
			SrcWatermarkLoc = NextPoint;
			if(AlignTargetBoneAlongAxesMap.Contains(TargetSkeleton.BoneNames[TgtBoneIndex]))
			{
				auto const &TargetDir = (TargetSkeleton.RetargetGlobalPose[TgtNextBoneIndex].GetTranslation() - TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetTranslation()).GetSafeNormal();
				RefineDirection(TargetSkeleton.BoneNames[TgtBoneIndex], TargetDir, SourceDir);
			}
			if(TargetSkeleton.AlignBoneWithDirection(TgtBoneIndex, TgtNextBoneIndex, SourceDir, OutDeltas))
			{
				ChangedCount++;
			}
			TgtId++;
		}
	}
	return ChangedCount;
}
bool FEPRetargeter::MatchLastBoneInChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton, TMap<FName, FName> const &EPChainMapping)
{
	if(ChainIndex < 0 && ChainIndex >= FMath::Min(TargetSkeleton.EPChains.Num(), SourceSkeleton.EPChains.Num()))
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Invalid Chain index: %d (%d - %d)."), ChainIndex, SourceSkeleton.EPChains.Num(), TargetSkeleton.EPChains.Num());
		return false;
	}
	auto TgtChain = TargetSkeleton.EPChains[ChainIndex];
	auto SrcChain = SourceSkeleton.EPChains[ChainIndex];
	auto SrcBoneIndex = SrcChain.BoneIndices.Last();
	auto TgtBoneIndex = TgtChain.BoneIndices.Last();
	if(SrcBoneIndex == 0 || TgtBoneIndex == 0)
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore root bone: %s (%s) - %s (%s)."), *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[0].ToString(), *SrcChain.Name.ToString(), *SourceSkeleton.BoneNames[0].ToString());
		return false;
	}
	if(SrcChain.Children.IsEmpty() || TgtChain.Children.IsEmpty())
	{
		if((MatchFlag & static_cast<uint32>(Matching::EFlag::LeafBone)) == 0)
		{
			UE_LOG(EasyPose, Verbose, TEXT("Ignore leaf-bone: %s (%s)."), *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
			return false;
		}
		return MatchLastBoneInLeafChain(OutDeltas, ChainIndex, SourceSkeleton, TargetSkeleton);
	}
	if((MatchFlag & static_cast<uint32>(Matching::EFlag::SubChain)) == 0)
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore sub-chain: %s (%s)."), *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
		return false;
	}
	auto Result = false;
	auto SrcSortedChildrenBoneIndices = SourceSkeleton.GetSortedChildrenIndicesByMostAlignedWithMedian(ChainIndex);
	auto TgtSortedChildrenIndices = TargetSkeleton.GetSortedChildrenIndicesByMostAlignedWithMedian(ChainIndex);
	if(SrcSortedChildrenBoneIndices.IsEmpty() || TgtSortedChildrenIndices.IsEmpty())
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore sub-chain after checking symmetric: %s (%s)."), *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
		return false;
	}
	auto SrcMostAlignedChildBoneIndex = SrcSortedChildrenBoneIndices[0];
	auto SrcSubChainName = SourceSkeleton.ChainThatContainsBone[SrcMostAlignedChildBoneIndex];
	if(!EPChainMapping.Contains(SrcSubChainName))
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Missing Source chain: %s."), *SrcSubChainName.ToString());
		return false;
	}
	auto TgtSubChainName = EPChainMapping[SrcSubChainName];
	auto TgtMostAlignedChildBoneIndex = TargetSkeleton.EPChains[TargetSkeleton.EPChainMap[TgtSubChainName]].BoneIndices[0];
	FVector Direction = (SourceSkeleton.RetargetGlobalPose[SrcMostAlignedChildBoneIndex].GetTranslation() - SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetTranslation()).GetSafeNormal();
	if(AlignTargetBoneAlongAxesMap.Contains(TargetSkeleton.BoneNames[TgtBoneIndex]))
	{
		auto const &TargetDir = (TargetSkeleton.RetargetGlobalPose[TgtMostAlignedChildBoneIndex].GetTranslation() - TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetTranslation()).GetSafeNormal();
		RefineDirection(TargetSkeleton.BoneNames[TgtBoneIndex], TargetDir, Direction);
	}
	if(TargetSkeleton.AlignBoneWithDirection(TgtBoneIndex, TgtMostAlignedChildBoneIndex, Direction, OutDeltas))
	{
		UE_LOG(EasyPose, Verbose, TEXT("Matched sub-chain: %s (%s) - %s (%s)")
				, *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString()
				, *TgtSubChainName.ToString(), *TargetSkeleton.BoneNames[TgtMostAlignedChildBoneIndex].ToString()
				);
		Result = true;
	}
	if((MatchFlag & static_cast<uint32>(Matching::EFlag::MultiSubChain)) != 0 && SrcSortedChildrenBoneIndices.Num() > 1)
	{
		auto RotationAxis = (TargetSkeleton.RetargetGlobalPose[TgtMostAlignedChildBoneIndex].GetTranslation() - TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetTranslation()).GetSafeNormal();
		auto SrcSecondAlignedChildBoneIndex = SrcSortedChildrenBoneIndices[1];
		SrcSubChainName = SourceSkeleton.ChainThatContainsBone[SrcSecondAlignedChildBoneIndex];
		if(!EPChainMapping.Contains(SrcSubChainName))
		{
			FEasyPoseModule::sShowWarning |= 2;
			UE_LOG(EasyPose, Error, TEXT("2nd Missing Source chain: %s."), *SrcSubChainName.ToString());
			return false;
		}
		TgtSubChainName = EPChainMapping[SrcSubChainName];
		auto TgtSecondAlignedChildBoneIndex = TargetSkeleton.EPChains[TargetSkeleton.EPChainMap[TgtSubChainName]].BoneIndices[0];
		auto FromNormDir = (TargetSkeleton.RetargetGlobalPose[TgtSecondAlignedChildBoneIndex].GetTranslation() - TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetTranslation()).GetSafeNormal();
		auto ToNormDir = (SourceSkeleton.RetargetGlobalPose[SrcSecondAlignedChildBoneIndex].GetTranslation() - SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetTranslation()).GetSafeNormal();
		auto FromNormPlane = FVector::CrossProduct(RotationAxis, FromNormDir).GetSafeNormal();
		auto ToNormPlane = FVector::CrossProduct(RotationAxis, ToNormDir).GetSafeNormal();
		auto Delta = FQuat::FindBetweenNormals(FromNormPlane, ToNormPlane);
		if(auto LocalDelta = TargetSkeleton.ApplyRotation(Delta, TgtBoneIndex, true); LocalDelta.IsSet())
		{
			OutDeltas[TgtBoneIndex] = (OutDeltas[TgtBoneIndex] * LocalDelta.GetValue()).GetNormalized();
			UE_LOG(EasyPose, Verbose, TEXT("Matched Multiple: %s (%s) - %s (%s)")
					, *TgtChain.Name.ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString()
					, *TgtSubChainName.ToString(), *TargetSkeleton.BoneNames[TgtSecondAlignedChildBoneIndex].ToString()
					);
			Result = true;
		}
	}
	else
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore multi sub-chain: %s (%d)."), *TgtChain.Name.ToString(), SrcSortedChildrenBoneIndices.Num());
	}
	return Result;
}
bool FEPRetargeter::MatchLastBoneInLeafChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton)
{
	if(ChainIndex < 0 && ChainIndex >= FMath::Min(TargetSkeleton.EPChains.Num(), SourceSkeleton.EPChains.Num()))
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Invalid Chain index: %d (%d - %d)."), ChainIndex, SourceSkeleton.EPChains.Num(), TargetSkeleton.EPChains.Num());
		return false;
	}
	auto &TgtChain = TargetSkeleton.EPChains[ChainIndex];
	auto &SrcChain = SourceSkeleton.EPChains[ChainIndex];
	if(!SrcChain.Children.IsEmpty() || !TgtChain.Children.IsEmpty())
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore, not a leaf chain: %s (%d) - %s (%d).")
			, *TgtChain.Name.ToString(), TgtChain.Children.Num()
			, *SrcChain.Name.ToString(), SrcChain.Children.Num()
			);
		return false;
	}
	auto SrcBoneIndex = SrcChain.BoneIndices.Last();
	auto TgtBoneIndex = TgtChain.BoneIndices.Last();
	auto const &SrcGlobalRotation = SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetRotation();
	auto const &TgtGlobalRotation = TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetRotation();
	int32 SrcRefBoneIndex = SourceSkeleton.ParentIndices[SrcBoneIndex];
	int32 TgtRefBoneIndex = TargetSkeleton.ParentIndices[TgtBoneIndex];
	if(SrcRefBoneIndex == INDEX_NONE || TgtRefBoneIndex == INDEX_NONE)
	{
		UE_LOG(EasyPose, Warning, TEXT("Leaf bone without reference: %s (%d) - %s (%d).")
				, *SrcChain.Name.ToString(), SrcRefBoneIndex
				, *TgtChain.Name.ToString(), TgtRefBoneIndex
				);
		return false;
	}
	constexpr float Tolerance = 0.01f;
	float RefZDotProd, ZDotProd;
	auto IsRefZAligned = EPUtils::IsAligned<2>(SourceSkeleton.RetargetGlobalPose[SrcRefBoneIndex].GetRotation(), TargetSkeleton.RetargetGlobalPose[TgtRefBoneIndex].GetRotation(), Tolerance, &RefZDotProd);
	auto IsZAxisAligned = EPUtils::IsAligned<2>(SourceSkeleton.RetargetGlobalPose[SrcBoneIndex].GetRotation(), TargetSkeleton.RetargetGlobalPose[TgtBoneIndex].GetRotation(), Tolerance, &ZDotProd);
	UE_LOG(EasyPose, VeryVerbose, TEXT("Matching leaf: %s (%s) - %s (%s), IsRefZAligned: %d (%f), IsZAxisAligned: %d (%f).")
		, *SourceSkeleton.BoneNames[SrcRefBoneIndex].ToString()
		, *SourceSkeleton.RetargetGlobalPose[SrcRefBoneIndex].GetRotation().ToString()
		, *TargetSkeleton.BoneNames[TgtRefBoneIndex].ToString()
		, *TargetSkeleton.RetargetGlobalPose[TgtRefBoneIndex].GetRotation().ToString()
		, IsRefZAligned, RefZDotProd, IsZAxisAligned, ZDotProd);
	if (IsRefZAligned && IsZAxisAligned)
	{
		FQuat Delta = (TgtGlobalRotation.Inverse() * SrcGlobalRotation).GetNormalized();
		if(AlignTargetBoneAlongAxesMap.Contains(TargetSkeleton.BoneNames[TgtBoneIndex]))
		{
			auto AlongAxes = AlignTargetBoneAlongAxesMap[TargetSkeleton.BoneNames[TgtBoneIndex]];
			FRotator Rotator = Delta.Rotator();
			uint8 IgnoreAxes = !AlongAxes;
			if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::X))
			{
				UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignore X."), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
				Rotator.Pitch = 0;
			}
			if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::Y))
			{
				UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignore Y."), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
				Rotator.Roll = 0;
			}
			if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::Z))
			{
				UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignore Z."), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
				Rotator.Yaw = 0;
			}
			Delta = Rotator.Quaternion();
		}
		if(auto LocalDelta = TargetSkeleton.ApplyRotation(Delta, TgtBoneIndex, false); LocalDelta.IsSet())
		{
			OutDeltas[TgtBoneIndex] = (OutDeltas[TgtBoneIndex] * LocalDelta.GetValue()).GetNormalized();
			UE_LOG(EasyPose, Verbose, TEXT("Matched Leaf: %s - %s."), *SourceSkeleton.BoneNames[SrcBoneIndex].ToString(), *TargetSkeleton.BoneNames[TgtBoneIndex].ToString());
			return true;
		}
	}
	else
	{
		UE_LOG(EasyPose, Verbose, TEXT("Ignore leaf: %s, Ref: %d, Z: %d."), *TgtChain.Name.ToString(), IsRefZAligned, IsZAxisAligned);
	}
	return false;
}
bool FEPRetargetSkeleton::AlignBoneWithDirection(int32 BoneStartIndex, int32 BoneEndIndex, FVector const &NormalDirection, TArray<FQuat> &OutDeltas)
{
	FVector CurrentDir = (RetargetGlobalPose[BoneEndIndex].GetTranslation() - RetargetGlobalPose[BoneStartIndex].GetTranslation()).GetSafeNormal();
	FQuat Delta = FQuat::FindBetweenNormals(CurrentDir, NormalDirection);
	if(auto LocalDelta = ApplyRotation(Delta, BoneStartIndex, true); LocalDelta.IsSet())
	{
		OutDeltas[BoneStartIndex] = (OutDeltas[BoneStartIndex] * LocalDelta.GetValue()).GetNormalized();
		return true;
	}
	return false;
}
TOptional<FQuat> FEPRetargetSkeleton::ApplyRotation(FQuat const &Delta, int32 BoneIndex, bool NeedBoneSpaceGlobalTransform)
{
	if(EPUtils::IsEmptyOrIdentity(Delta, DELTA_TOLERANCE))
	{
		return NullOpt;
	}
	if(BoneIndex < 0)
	{
		FEasyPoseModule::sShowWarning |= 2;
		UE_LOG(EasyPose, Error, TEXT("Invalid Bone Index: %d"), BoneIndex);
		return NullOpt;
	}
	auto LocalDelta = NeedBoneSpaceGlobalTransform ? (RetargetGlobalPose[BoneIndex].GetRotation().Inverse() * Delta * RetargetGlobalPose[BoneIndex].GetRotation()).GetNormalized() : Delta;
	FQuat const LocalBoneRotation = (RetargetLocalPose[BoneIndex].GetRotation() * LocalDelta).GetNormalized();
	RetargetLocalPose[BoneIndex].SetRotation(LocalBoneRotation);
	UpdateGlobalTransformsBelowBone(BoneIndex);
	return TOptional<FQuat>(MoveTemp(LocalDelta));
}
void FEPRetargeter::RefineDirection(FName const &BoneName, FVector const &OrgDir, FVector &OutNewDir) const
{
	auto AlongAxes = AlignTargetBoneAlongAxesMap[BoneName];
	auto IgnoreAxes = ~AlongAxes;
	if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::Z))
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignores Z."), *BoneName.ToString());
		OutNewDir = EPUtils::GetNormalVectorProjectedOnPlaneAB(OrgDir, FVector::UnitZ(), OutNewDir);
	}
	if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::Y))
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignores Y."), *BoneName.ToString());
		OutNewDir = EPUtils::GetNormalVectorProjectedOnPlaneAB(OrgDir, FVector::UnitY(), OutNewDir);
	}
	if(IgnoreAxes & StaticCast<uint8>(EAlongAxes::X))
	{
		UE_LOG(EasyPose, VeryVerbose, TEXT("%s ignores X."), *BoneName.ToString());
		OutNewDir = EPUtils::GetNormalVectorProjectedOnPlaneAB(OrgDir, FVector::UnitX(), OutNewDir);
	}
}
