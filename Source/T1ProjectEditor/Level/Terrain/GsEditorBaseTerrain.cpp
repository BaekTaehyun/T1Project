// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorBaseTerrain.h"
#include <EngineGlobals.h>
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Public/UnrealClient.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Public/UnrealEdGlobals.h"
#include "Editor/ComponentVisualizers/Public/SplineComponentVisualizer.h"
#include "Editor/UnrealEd/Classes/Editor/UnrealEdEngine.h"
#include "T1ProjectEditor/Widget/GsEditorTerrainWidget.h"
#include "T1ProjectEditor/Level/Terrain/GsEditorTerrainPillarComp.h"
#include "T1ProjectEditor/Level/Terrain/GsEditorTerrainPlaneComp.h"

// Sets default values
AGsEditorBaseTerrain::AGsEditorBaseTerrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	_Root->SetRelativeLocation(FVector::ZeroVector);
	RootComponent = _Root;

	_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	_Spline->SetRelativeLocation(FVector::ZeroVector);
	_Spline->EditorUnselectedSplineSegmentColor = FColor::Blue;
	_Spline->EditorSelectedSplineSegmentColor = FColor::Yellow;

	_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	SetActorHiddenInGame(true);

#if WITH_EDITOR
	/*USelection::SelectObjectEvent.AddUObject(this, &AGsEditorBaseTerrain::OnSelectCallback);
	USelection::SelectNoneEvent.AddUObject(this, &AGsEditorBaseTerrain::OnUnSelectCallback);*/
#endif
}

#if WITH_EDITOR
bool AGsEditorBaseTerrain::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

void AGsEditorBaseTerrain::Tick(float in_delta)
{
	Super::Tick(in_delta);

#if WITH_EDITOR
	if (_Widget)
	{
		if (_Widget->bVisible)
		{
			FLevelEditorViewportClient* Client = (FLevelEditorViewportClient*)(GEditor->GetActiveViewport()->GetClient());

			if (Client)
			{
				FRotator rot = UKismetMathLibrary::FindLookAtRotation(_Widget->GetComponentLocation(), Client->GetViewLocation());

				_Widget->SetWorldRotation(rot);
			}
		}
	}
#endif
}

void AGsEditorBaseTerrain::InitPoints()
{
	switch (_ShapeType)
	{
	case ETerrainShapeType::Polygon:
		InitPolygon();
		break;
	case ETerrainShapeType::Circle:
		InitCircle();
		break;
	case ETerrainShapeType::Line:
		InitLine();
		break;
	default:
		break;
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
	switch (_ShapeType)
	{
	case ETerrainShapeType::Polygon:
		DrawPolygon();
		break;
	case ETerrainShapeType::Circle:
		DrawCircle();
		break;
	case ETerrainShapeType::Line:
		DawLine();
		break;
	default:
		break;
	}

	SetWidgetPosition();
	SetWidgetText();
}

void AGsEditorBaseTerrain::DrawPolygon()
{
	DrawPlillar();
	DrawPlanes();
}

void AGsEditorBaseTerrain::DrawCircle()
{
	DrawPlillar();
	DrawPlanes();
}

void AGsEditorBaseTerrain::DawLine()
{
	DrawPlillar();
	DrawPlanes(false);
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

void AGsEditorBaseTerrain::DrawPlanes(bool in_close)
{
	int num = _PillarArray.Num();
	int last = num - 1;

	if (_PlaneArray.Num() > 0
		&& _PlaneArray.IsValidIndex(last))
	{
		for (int i = 0; i < num; ++i)
		{
			if (i == last)
			{
				if (in_close)
				{
					if (_PlaneArray[last])
					{
						if (_PillarArray[last] && _PillarArray[0])
						{
							_PlaneArray[last]->Draw(_PillarArray[last], _PillarArray[0], _Height, _PlaneOuterColor, _PlaneInsideColor);
						}
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

void AGsEditorBaseTerrain::DrawPlillar()
{
	UGsEditorTerrainPillarComp* pillar;
	int num = _PointArray.Num();

	for (int i = 0; i < num; ++i)
	{
		pillar = _PillarArray[i];

		if (pillar)
		{
			pillar->SetWorldLocation(_PointArray[i]);
			pillar->Draw(_PillarColor, _Height);
		}
	}
}

void AGsEditorBaseTerrain::SetWidgetPosition()
{
	if (_Widget)
	{
		int32 num = _PointArray.Num();
		FVector pos;

		if (num > 0)
		{
			float x = 0;
			float y = 0;
			float z = MIN_int32;

			for (FVector& iter : _PointArray)
			{
				x += iter.X;
				y += iter.Y;

				if (iter.Z > z)
				{
					z = iter.Z;
				}
			}

			x /= num;
			y /= num;

			pos = FVector::FVector(x, y, z + _Height + _WidgetHeight);
			_Widget->SetWorldLocation(pos);
		}
		else
		{
			pos = GetActorLocation() + FVector(0, 0, _Height + _WidgetHeight);
			_Widget->SetWorldLocation(pos);
		}
	}
}

void AGsEditorBaseTerrain::SetWidgetText()
{
	UUserWidget* widget = _Widget->GetUserWidgetObject();

	if (widget)
	{
		UGsEditorTerrainWidget* terrainWidget = Cast<UGsEditorTerrainWidget>(widget);

		if (terrainWidget)
		{
			if (terrainWidget->_ContentsText)
			{
				terrainWidget->_ContentsText->SetText(FText::FromString(_Tag));
			}
		}
	}
}

#if WITH_EDITOR
void AGsEditorBaseTerrain::OnSelectCallback(UObject* in_object)
{
	if (_Widget)
	{
		if (this == in_object)
		{
			_Widget->SetVisibility(true, true);
			UE_LOG(LogTemp, Log, TEXT("Select : %s"), *GetName());
		}
		else
		{
			_Widget->SetVisibility(false, true);
			UE_LOG(LogTemp, Log, TEXT("Unelect : %s"), *GetName());
		}
	}
}

void AGsEditorBaseTerrain::OnUnSelectCallback()
{
	if (_Widget)
	{
		_Widget->SetVisibility(false, true);
	}
}
#endif

void AGsEditorBaseTerrain::InitPolygon()
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
			_Spline->AddSplinePoint(origin + FVector(100, 0, 0), ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(0, 100, 0), ESplineCoordinateSpace::World);
		}

		InitPointArray();
	}
}

void AGsEditorBaseTerrain::InitCircle()
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

	if (TryGetSelectedIndexInSpline(out_selectIndex))
	{		
		if (_Spline->GetNumberOfSplinePoints() >= out_selectIndex)
		{
			location = _Spline->GetWorldLocationAtSplinePoint(out_selectIndex);
			distance = FVector::Distance(origin, location);
		}		

		UE_LOG(LogTemp, Log, TEXT("Selected spline index : %d"), out_selectIndex);
	}
	else
	{		
		float accum = 0;

		if (_PointArray.Num() > 0)
		{
			for (FVector& iter : _PointArray)
			{
				accum += FVector::Distance(origin, iter);
			}

			distance = accum / _PointArray.Num();
		}
		else
		{
			distance = DEFAULT_TERRAIN_DISTANCE;
		}		
	}

	UE_LOG(LogTemp, Log, TEXT("Spline distance : %f"), distance);

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
			direction = FVector::ForwardVector.RotateAngleAxis(degree, FVector::UpVector) * distance;
			_Spline->AddSplinePoint(origin + direction, ESplineCoordinateSpace::World);
		}
	}

	InitPointArray();
}

void AGsEditorBaseTerrain::InitLine()
{
	if (_Spline)
	{
		_Spline->SetClosedLoop(false);
	}

	if (_Spline)
	{
		int num = _Spline->GetNumberOfSplinePoints();

		if (num < DEFAULT_LINE_POINT_NUM)
		{
			FVector origin = GetActorLocation();

			_Spline->ClearSplinePoints();

			_Spline->AddSplinePoint(origin, ESplineCoordinateSpace::World);
			_Spline->AddSplinePoint(origin + FVector(100, 0, 0), ESplineCoordinateSpace::World);
		}

		InitPointArray();
	}
}

void AGsEditorBaseTerrain::InitPointArray()
{
	_PointArray.Empty();
	int num = _Spline->GetNumberOfSplinePoints();

	for (int i = 0; i < num; ++i)
	{
		_PointArray.Add(_Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}

bool AGsEditorBaseTerrain::TryGetSelectedIndexInSpline(int32& out_index)
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