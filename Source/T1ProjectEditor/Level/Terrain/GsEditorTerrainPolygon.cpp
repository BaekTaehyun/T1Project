// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainPolygon.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

void AGsEditorTerrainPolygon::InitShape()
{
	if (_Spline)
	{
		_Spline->SetClosedLoop(true);
	}

	if (_Spline)
	{
		int num = _Spline->GetNumberOfSplinePoints();

		if (num < DEFAULT_POLYGON_POINT_NUM)
		{
			FVector origin = GetActorLocation();

			_Spline->ClearSplinePoints();

			_Spline->AddSplinePoint(origin, ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(DEFAULT_TERRAIN_DISTANCE, 0, 0), ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(0, DEFAULT_TERRAIN_DISTANCE, 0), ESplineCoordinateSpace::World);
		}

		InitPointArray();
	}
}

void AGsEditorTerrainPolygon::DrawShape()
{
	DrawPlillar();
	DrawPlanes();
}