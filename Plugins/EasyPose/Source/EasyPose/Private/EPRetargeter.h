// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "EasyPoseSettings.h"
#include "Retargeter/IKRetargeter.h"
enum class MatchMode : uint8 {
	SIMILAR, DYNAMIC, EXACT
};
namespace Matching {
enum class EStep : uint8 {
	SubChain, MultiSubChain, LeafBone, Max
};
enum class EFlag : uint32 {
	SubChain = 1u << static_cast<uint8>(EStep::SubChain),
	MultiSubChain = 1u << static_cast<uint8>(EStep::MultiSubChain),
	LeafBone = 1u << static_cast<uint8>(EStep::LeafBone),
	All = (SubChain | MultiSubChain | LeafBone)
};
}
struct FEPChain
{
    FName Name = NAME_None;
    TArray<int32> BoneIndices;
	FEPChain *Parent = nullptr;
    TArray<FEPChain*> Children;
    TArray<float> ContiguousBoneLengths;
	void Dump() const;
};
struct FEPRetargetSkeleton
{
	TArray<FName> BoneNames;
	TArray<int32> ParentIndices;
	TArray<FTransform> RetargetLocalPose;
	TArray<FTransform> RetargetGlobalPose;
	FName RetargetPoseName;
	USkeletalMesh* SkeletalMesh;
	int32 RetargetRootBoneIndex = INDEX_NONE;
	TArray<FName> ChainThatContainsBone;
	TArray<FEPChain> EPChains;
	TMap<FName, int32> EPChainMap;
	void Dump() const;
	TOptional<FEPChain> InvalidateBoneChain(FBoneChain const &BoneChain);
	bool IsRootBoneConnectToParentTail(FEPChain const &Chain) const;
	bool AlignBoneWithDirection(int32 BoneStartIndex, int32 BoneEndIndex, FVector const &NormalDirection, TArray<FQuat> &OutDeltas);
	TOptional<FQuat> ApplyRotation(FQuat const &Delta, int32 BoneIndex, bool NeedBoneSpaceGlobalTransform);
	FEPChain *GetEPChainParent(const FEPChain &Chain);
	inline void AddEPChain(FEPChain EPChain)
	{
		EPChainMap.Add(EPChain.Name, EPChains.Num());
		EPChains.Add(MoveTemp(EPChain));
	}
	inline FEPChain *GetEPChainByName(FName const &Name)
	{
		if(EPChainMap.Find(Name))
		{
			return &EPChains[EPChainMap[Name]];
		}
		else
		{
			return nullptr;
		}
	}
	inline FEPChain const *GetEPChainByName(FName const &Name) const
	{
		if(EPChainMap.Find(Name))
		{
			return &EPChains[EPChainMap[Name]];
		}
		else
		{
			return nullptr;
		}
	}
	inline FEPChain const *GetEPChainFromBoneIndex(int32 BoneId) const
	{
		return GetEPChainByName(ChainThatContainsBone[BoneId]);
	}
	inline FEPChain *GetEPChainFromBoneIndex(int32 BoneId)
	{
		return GetEPChainByName(ChainThatContainsBone[BoneId]);
	}
	inline FEPChain *GetParentOfChain(FEPChain const &EPChain)
	{
		if(EPChain.BoneIndices.IsEmpty())
		{
			return nullptr;
		}
		auto RootBoneInChain = EPChain.BoneIndices[0];
		auto ParentIndex = ParentIndices[RootBoneInChain];
		while(ParentIndex != INDEX_NONE)
		{
			auto ParentChain = GetEPChainFromBoneIndex(ParentIndex);
			if(ParentChain != nullptr)
			{
				return ParentChain;
			}
			ParentIndex = ParentIndices[ParentIndex];
		}
		return nullptr;
	}
	inline FEPChain const *GetParentOfChain(FEPChain const &EPChain) const
	{
		if(EPChain.BoneIndices.IsEmpty())
		{
			return nullptr;
		}
		auto RootBoneInChain = EPChain.BoneIndices[0];
		auto ParentIndex = ParentIndices[RootBoneInChain];
		while(ParentIndex != INDEX_NONE)
		{
			auto ParentChain = GetEPChainFromBoneIndex(ParentIndex);
			if(ParentChain != nullptr)
			{
				return ParentChain;
			}
			ParentIndex = ParentIndices[ParentIndex];
		}
		return nullptr;
	}
	TArray<int32> GetSortedChildrenIndicesByMostAlignedWithMedian(int32 ChainIndex) const;
	void Initialize(USkeletalMesh* InSkeletalMesh, FName const &RetargetRootBoneName);
	void GenerateRetargetPose(const FIKRetargetPose* InRetargetPose);
	void Reset();
	int32 FindBoneIndexByName(const FName InName) const;
	int32 GetParentIndex(const int32 BoneIndex) const;
	void UpdateGlobalTransformsBelowBone(const int32 StartBoneIndex);
	void UpdateGlobalTransformsBelowBone(
		const int32 StartBoneIndex,
		const TArray<FTransform>& InLocalPose,
		TArray<FTransform>& OutGlobalPose) const;
	void UpdateLocalTransformsBelowBone(
		const int32 StartBoneIndex,
		TArray<FTransform>& OutLocalPose,
		const TArray<FTransform>& InGlobalPose) const;
	void UpdateGlobalTransformOfSingleBone(
		const int32 BoneIndex,
		const TArray<FTransform>& InLocalPose,
		TArray<FTransform>& OutGlobalPose) const;
	void UpdateLocalTransformOfSingleBone(
		const int32 BoneIndex,
		TArray<FTransform>& OutLocalPose,
		const TArray<FTransform>& InGlobalPose) const;
	FTransform GetGlobalRefPoseOfSingleBone(
		const int32 BoneIndex,
		const TArray<FTransform>& InGlobalPose) const;
	int32 GetCachedEndOfBranchIndex(const int32 InBoneIndex) const;
	void GetChildrenIndices(const int32 BoneIndex, TArray<int32>& OutChildren) const;
	void GetChildrenIndicesRecursive(const int32 BoneIndex, TArray<int32>& OutChildren) const;
	bool IsParentOfChild(const int32 PotentialParentIndex, const int32 ChildBoneIndex) const;
private:
	mutable TArray<int32> CachedEndOfBranchIndices;
};
struct FEPResolvedBoneChain
{
	FEPResolvedBoneChain(const FBoneChain& BoneChain, const FEPRetargetSkeleton& Skeleton, TArray<int32> &OutBoneIndices);
	bool bFoundStartBone = false;
	bool bFoundEndBone = false;
	bool bEndIsStartOrChildOfStart  = false;
	inline bool IsValid() const
	{
		return bFoundStartBone && bFoundEndBone && bEndIsStartOrChildOfStart;
	}
};
struct FEPRetargeter
{
	FEPRetargeter(UIKRetargeter *IKRetargeter);
    FEPRetargetSkeleton EPRetargetSkeletons[2];
	TMap<FName, FName> EPChainMappingList[2];
	TArray<FQuat> Deltas;
	TSet<FName> IgnoreTargetChain;
	TMap<FName, uint8> AlignTargetBoneAlongAxesMap;
	uint32 MatchFlag;
	void Init(class UEasyPoseSettings const *EPSettings);
	TOptional<FName> FindMappedChainWithSource(TArray<TObjectPtr<URetargetChainSettings>> const &AllChainSettings, FName const &Source) const;
	void InvalidateChainMapping(TArray<TObjectPtr<URetargetChainSettings>> const &AllChainSettings, const TArray<FBoneChain> &SourceIKRigChains, const TArray<FBoneChain> &TargetIKRigChains);
	int32 MatchPose(UIKRetargeter *IKRetargeter, ERetargetSourceOrTarget RetargetDestination, uint32 InMatchFlag);
	int32 MatchChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton, MatchMode MatchingMode);
	bool MatchLastBoneInChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton, TMap<FName, FName> const &EPChainMapping);
	bool MatchLastBoneInLeafChain(TArray<FQuat> &OutDeltas, int32 ChainIndex, FEPRetargetSkeleton const &SourceSkeleton, FEPRetargetSkeleton &TargetSkeleton);
	void RefineDirection(FName const &BoneName, FVector const &TargetDir, FVector &Direction) const;
	void Dump() const;
	TWeakPtr<SWidget> WarningBox;
};
