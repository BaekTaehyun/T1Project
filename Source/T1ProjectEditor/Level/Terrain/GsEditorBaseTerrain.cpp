// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorBaseTerrain.h"
#include "./GsEditorTerrainPillarComp.h"
#include "./GsEditorTerrainPlaneComp.h"
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

// Sets default values
AGsEditorBaseTerrain::AGsEditorBaseTerrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	_Root->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = _Root;

	_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	_Spline->SetRelativeLocation(FVector::ZeroVector);
	_Spline->EditorUnselectedSplineSegmentColor = FColor::Blue;
	_Spline->EditorSelectedSplineSegmentColor = FColor::Yellow;	

	SetActorHiddenInGame(true);
}

void AGsEditorBaseTerrain::InitPoints()
{	
	if (_Spline)
	{
		int num = _Spline->GetNumberOfSplinePoints();

		if (num < 3)
		{
			FVector origin = GetActorLocation();

			_Spline->ClearSplinePoints();

			_Spline->AddSplinePoint(origin, ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(100, 0, 0), ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(0, 100, 0), ESplineCoordinateSpace::World);
		}

		_PointArray.Empty();
		num = _Spline->GetNumberOfSplinePoints();

		for (int i = 0; i < num; ++i)
		{
			_PointArray.Add(_Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
		}
	}
}

void AGsEditorBaseTerrain::DestoryAllComponents()
{
	const TArray<UActorComponent*>& pillarArray = GetComponentsByClass(UGsEditorTerrainPillarComp::StaticClass());

	for (UActorComponent* iter : pillarArray)
	{
		if (iter)
		{
			iter->UnregisterComponent();
			iter->DestroyComponent();
		}		
	}

	_PillarArray.Empty();

	const TArray<UActorComponent*>& planeArray = GetComponentsByClass(UGsEditorTerrainPlaneComp::StaticClass());

	for (UActorComponent* iter : planeArray)
	{
		if (iter)
		{
			iter->UnregisterComponent();
			iter->DestroyComponent();
		}		
	}

	_PlaneArray.Empty();
}

void AGsEditorBaseTerrain::Draw()
{
	UGsEditorTerrainPillarComp* pillar;
	int num = _PointArray.Num();

	for(int i = 0; i < num; ++i)
	{
		pillar = _PillarArray[i];		

		if (pillar)
		{
			pillar->SetWorldLocation(_PointArray[i]);
			pillar->Draw(_PillarColor, _Height);
		}
	}

	num = _PillarArray.Num();
	int last = num - 1;	

	if (_PlaneArray.Num() > 0
		&& _PlaneArray.IsValidIndex(last))
	{
		for (int i = 0; i < num; ++i)
		{
			if (i == last)
			{
				if (_PlaneArray[last])
				{
					if (_PillarArray[last] && _PillarArray[0])
					{
						_PlaneArray[last]->Draw(_PillarArray[last], _PillarArray[0], _Height, _PlaneOuterColor, _PlaneInsideColor);
					}
				}
			}
			else
			{
				if (_PlaneArray[i])
				{
					if (_PillarArray[i] && _PillarArray[i + 1])
					{
						_PlaneArray[i]->Draw(_PillarArray[i], _PillarArray[i + 1], _Height, _PlaneOuterColor, _PlaneInsideColor);
					}
				}
			}
		}
	}
}

void AGsEditorBaseTerrain::RegisterPillar(UGsEditorTerrainPillarComp* in_pillar, int32 in_index)
{
	if (in_pillar)
	{
		_PillarArray.Add(in_pillar);
		in_pillar->_Parent = this;		
	}
}

void AGsEditorBaseTerrain::RegisterPlane(UGsEditorTerrainPlaneComp* in_plane)
{
	if (in_plane)
	{
		_PlaneArray.Add(in_plane);
		in_plane->_Parent = this;
	}
}

FVector AGsEditorBaseTerrain::GetCenterBetweenPoints(int32 in_start, int32 in_end)
{
	if (_PillarArray.IsValidIndex(in_start)
		&& _PillarArray.IsValidIndex(in_end))
	{
		FVector start = _PillarArray[in_start]->GetComponentLocation();
		FVector end = _PillarArray[in_end]->GetComponentLocation();

		return ((start + end) / 2.0f);
	}

	return FVector::ZeroVector;
}