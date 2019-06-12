// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainLine.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

AGsEditorTerrainLine::AGsEditorTerrainLine()
{
	if (_Spline)
	{
		_Spline->SetClosedLoop(false);
	}
}

void AGsEditorTerrainLine::InitShape()
{
	if (_Spline)
	{
		int num = _Spline->GetNumberOfSplinePoints();

		if (num < DEFAULT_LINE_POINT_NUM)
		{
			FVector origin = GetActorLocation();

			_Spline->ClearSplinePoints();

			_Spline->AddSplinePoint(origin, ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(DEFAULT_TERRAIN_DISTANCE, 0, 0), ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(DEFAULT_TERRAIN_DISTANCE * 2, 0, 0), ESplineCoordinateSpace::World);
		}

		InitPointArray();
	}
}

void AGsEditorTerrainLine::DrawShape()
{
	DrawPlillar();
	DrawPlanes(false);
}