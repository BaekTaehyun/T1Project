// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainShape.h"
#include "T1ProjectEditor/Widget/GsEditorWidget.h"
#include "T1ProjectEditor/Level/Terrain/GsEditorTerrainPillarComp.h"
#include "T1ProjectEditor/Level/Terrain/GsEditorTerrainPlaneComp.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

AGsEditorTerrainShape::AGsEditorTerrainShape()
{
	_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));

	if (_Spline)
	{
		_Spline->SetRelativeLocation(FVector::ZeroVector);
		_Spline->EditorUnselectedSplineSegmentColor = FColor::Blue;
		_Spline->EditorSelectedSplineSegmentColor = FColor::Yellow;
	}	
}

void AGsEditorTerrainShape::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitShape();
	DestroyAllComponents();

	int num = _PointArray.Num();
	UGsEditorTerrainPillarComp* pillar;
	UGsEditorTerrainPlaneComp* plane;
	FString name;
	FString index;

	for (int i = 0; i < num; ++i)
	{
		index = FString::FromInt(i + 1);
		name = "Pillar" + index;

		pillar = NewObject<UGsEditorTerrainPillarComp>(this, FName(*name));

		if (pillar)
		{
			pillar->RegisterComponent();
			RegisterPillar(pillar);
		}		

		name = "Plane" + index;

		plane = NewObject<UGsEditorTerrainPlaneComp>(this, FName(*name));

		if (plane)
		{
			plane->RegisterComponent();
			RegisterPlane(plane);
		}		
	}

	//Transform 
	for (UGsEditorTerrainPillarComp* iter : _PillarArray)
	{
		if (iter)
		{
			iter->AttachToComponent(_Root, FAttachmentTransformRules::KeepRelativeTransform);
		}		
	}

	for (UGsEditorTerrainPlaneComp* iter : _PlaneArray)
	{
		if (iter)
		{
			iter->AttachToComponent(_Root, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	DrawShape();
	InvalideWidgetPosition();
	InvalideWidgetText();
}

void AGsEditorTerrainShape::DestroyAllComponents()
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

	RegisterAllComponents();
}

void AGsEditorTerrainShape::RegisterPillar(UGsEditorTerrainPillarComp* in_pillar)
{
	if (in_pillar)
	{
		_PillarArray.Add(in_pillar);
		in_pillar->_Parent = this;
	}
}

void AGsEditorTerrainShape::RegisterPlane(UGsEditorTerrainPlaneComp* in_plane)
{
	if (in_plane)
	{
		_PlaneArray.Add(in_plane);
		in_plane->_Parent = this;
	}
}

FVector AGsEditorTerrainShape::GetCenterBetweenPoints(int32 in_start, int32 in_end)
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

void AGsEditorTerrainShape::InitPointArray()
{
	_PointArray.Empty();
	int num = _Spline->GetNumberOfSplinePoints();

	for (int i = 0; i < num; ++i)
	{
		_PointArray.Add(_Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}

void AGsEditorTerrainShape::DrawPlanes(bool in_close)
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

void AGsEditorTerrainShape::DrawPlillar()
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

void AGsEditorTerrainShape::InvalideWidgetPosition()
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
