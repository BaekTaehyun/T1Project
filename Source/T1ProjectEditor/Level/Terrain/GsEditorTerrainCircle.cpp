// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainCircle.h"
#include "Editor/UnrealEd/Classes/Editor/UnrealEdEngine.h"
#include "Editor/UnrealEd/Public/UnrealEdGlobals.h"
#include "Editor/ComponentVisualizers/Public/SplineComponentVisualizer.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"

void AGsEditorTerrainCircle::InitShape()
{
	if (nullptr == _Spline)
	{
		return;
	}

	_Spline->SetClosedLoop(true);

	FVector origin = GetActorLocation();
	float distance = 0;
	int num = _Spline->GetNumberOfSplinePoints();
	FVector location;
	int32 out_selectIndex;

#if WITH_EDITOR
	if (TryGetSelectedIndexInSpline(out_selectIndex))
	{
		if (_Spline->GetNumberOfSplinePoints() >= out_selectIndex)
		{
			location = _Spline->GetWorldLocationAtSplinePoint(out_selectIndex);
			_CircleDistance = FVector::Distance(origin, location);
		}

		UE_LOG(LogTemp, Log, TEXT("Selected spline index : %d"), out_selectIndex);
	}
#endif	

	if (_CircleDistance == 0)
	{
		_CircleDistance = DEFAULT_TERRAIN_DISTANCE;
	}

	UE_LOG(LogTemp, Log, TEXT("Spline distance : %f"), _CircleDistance);

	num = (num < DEFAULT_CIRCLE_POINT_NUM ? DEFAULT_CIRCLE_POINT_NUM : num);

	_Spline->ClearSplinePoints();

	if (num > 0)
	{
		float gap = 360.0f / num;
		float degree = 0;
		FVector direction;

		for (int i = 0; i < num; ++i)
		{
			degree = gap * i;
			direction = FVector::ForwardVector.RotateAngleAxis(degree, FVector::UpVector) * _CircleDistance;
			_Spline->AddSplinePoint(origin + direction, ESplineCoordinateSpace::World);
		}
	}

	InitPointArray();
}

void AGsEditorTerrainCircle::DrawShape()
{
	DrawPlillar();
	DrawPlanes();
}

#if WITH_EDITOR
bool AGsEditorTerrainCircle::TryGetSelectedIndexInSpline(int32& out_index)
{
	out_index = INVALIDE_INDEX;

	if (nullptr == GUnrealEd)
	{
		return false;
	}

	TSharedPtr<FSplineComponentVisualizer> splineVisualiser = StaticCastSharedPtr<FSplineComponentVisualizer>(GUnrealEd->FindComponentVisualizer(USplineComponent::StaticClass()));

	if (false == splineVisualiser.IsValid())
	{
		return false;
	}

	const TSet<int32>& indexArray = splineVisualiser.Get()->GetSelectedKeys();

	if (indexArray.Num() > 0)
	{
		out_index = indexArray.Array()[0];
		return true;
	}

	return false;
}
#endif