// Copyright longlt00502@gmail.com 2023. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Toolkits/ToolkitManager.h"
namespace EPUtils
{
	template <int AxisType>
	bool IsAligned(const FQuat &QA, const FQuat &QB, float Tolerance, float *OutDotProdResult = nullptr)
	{
		FVector ReferenceAxis;
		if constexpr (AxisType == 0)
			ReferenceAxis = FVector::UnitX();
		else if constexpr (AxisType == 1)
			ReferenceAxis = FVector::UnitY();
		else
			ReferenceAxis = FVector::UnitZ();
		FVector RotatedAxisQA = QA.RotateVector(ReferenceAxis);
		FVector RotatedAxisQB = QB.RotateVector(ReferenceAxis);
		float DotProdResult = FVector::DotProduct(RotatedAxisQA, RotatedAxisQB);
		if (OutDotProdResult != nullptr)
		{
			*OutDotProdResult = DotProdResult;
		}
		return 1 - DotProdResult <= Tolerance;
	}
	inline bool IsEmptyOrIdentity(FQuat const &Quat, float Tolerance=UE_KINDA_SMALL_NUMBER)
	{
		if (Quat.Equals(FQuat(0, 0, 0, 0), Tolerance))
		{
			UE_LOG(EasyPose, Error, TEXT("Ooops! Empty quaternion observed."));
			return true;
		}
		return Quat.Equals(FQuat::Identity, Tolerance);
	}
	inline FVector GetNormalVectorProjectedOnPlaneAB(FVector const &V, FVector const &Vpa, FVector const &Vpb)
	{
		FVector N = FVector::CrossProduct(Vpb, Vpa);
		FVector VProjectedOnN = (FVector::DotProduct(V, N) / FVector::DotProduct(N, N)) * N;
		return (V - VProjectedOnN).GetSafeNormal();
	}
	template <class FEditor, class UAssetType>
	TSharedPtr<FEditor> GetAssetEditorInstance(const UAssetType *Asset)
	{
		if (Asset == nullptr)
		{
			return nullptr;
		}
		TSharedPtr<IToolkit> AssetEditorInstance = FToolkitManager::Get().FindEditorForAsset(Asset);
		if (AssetEditorInstance.IsValid())
		{
			return StaticCastSharedPtr<FEditor>(AssetEditorInstance);
		}
		return nullptr;
	}
}