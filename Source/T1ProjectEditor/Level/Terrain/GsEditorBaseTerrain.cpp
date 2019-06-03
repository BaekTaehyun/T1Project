// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorBaseTerrain.h"
#include "./GsEditorTerrainPillarComp.h"
#include "./GsEditorTerrainPlaneComp.h"
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h"

// Sets default values
AGsEditorBaseTerrain::AGsEditorBaseTerrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGsEditorBaseTerrain::OnConstruction(const FTransform& in_transform)
{
	Super::OnConstruction(in_transform);

	_PillarArray.Empty();

	const TArray<UActorComponent*>& pillarArray = GetComponentsByClass(UGsEditorTerrainPillarComp::StaticClass());

	for (UActorComponent* iter : pillarArray)
	{
		if (iter)
		{
			RegisterPillar(Cast<UGsEditorTerrainPillarComp>(iter));
		}
	}

	_PlaneArray.Empty();

	const TArray<UActorComponent*>& planeArray = GetComponentsByClass(UGsEditorTerrainPlaneComp::StaticClass());

	for (UActorComponent* iter : planeArray)
	{
		if (iter)
		{
			RegisterPlane(Cast<UGsEditorTerrainPlaneComp>(iter));
		}
	}

	Draw();
}

// Called when the game starts or when spawned
void AGsEditorBaseTerrain::BeginPlay()
{
	Super::BeginPlay();

	Destroy();
}

void AGsEditorBaseTerrain::Draw()
{
	for (UGsEditorTerrainPillarComp* iter : _PillarArray)
	{
		if (iter)
		{
			iter->Draw();
		}
	}

	int num = _PillarArray.Num();
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
						_PlaneArray[last]->Draw(_PillarArray[last], _PillarArray[0]);
					}
				}
			}
			else
			{
				if (_PlaneArray[i])
				{
					if (_PillarArray[i] && _PillarArray[i + 1])
					{
						_PlaneArray[i]->Draw(_PillarArray[i], _PillarArray[i + 1]);
					}
				}
			}
		}
	}
}

void AGsEditorBaseTerrain::RegisterPillar(UGsEditorTerrainPillarComp* in_pillar)
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

void AGsEditorBaseTerrain::AddPillar(int32 in_start, int32 in_end)
{
	if (_PillarArray.IsValidIndex(in_start)
		&& _PillarArray.IsValidIndex(in_end))
	{
		int32 last = _PillarArray.Num() - 1;

		if ((in_start == last && in_end == 0)
			|| (in_end == last && in_start == 0))
		{
			TryCreate(in_start, in_end, -1);
		}
		else
		{
			int32 size = FMath::Abs(in_start - in_end);

			if (1 == size)
			{
				int32 upper = GetUpperIndex(in_start, in_end);

				TryCreate(in_start, in_end, upper);
			}
		}
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

bool AGsEditorBaseTerrain::TryCreatePillar(int32 in_index, FVector in_location)
{
	UGsEditorTerrainPillarComp* pillar = NewObject<UGsEditorTerrainPillarComp>();

	if (pillar)
	{
		pillar->SetWorldLocation(in_location);
		pillar->_Parent = this;

		if (-1 == in_index)
		{
			_PillarArray.Add(pillar);
		}
		else
		{
			_PillarArray.Insert(pillar, in_index);
		}
		
		//add pillar using sscss eidtor

		return true;					
	}

	return false;
}

bool AGsEditorBaseTerrain::TryCreate(int32 in_start, int32 in_end, int32 in_index)
{
	FVector center = GetCenterBetweenPoints(in_start, in_end);

	if (TryCreatePillar(in_index, center))
	{
		if (TryCreatePlane())
		{
			Draw();

			return true;
		}
	}

	return false;
}

bool AGsEditorBaseTerrain::TryCreatePlane()
{
	UGsEditorTerrainPlaneComp* plane = NewObject<UGsEditorTerrainPlaneComp>();

	if (plane)
	{
		_PlaneArray.Add(plane);
		plane->_Parent = this;

		//add plane using sscss eidtor

		return true;
	}

	return false;
}

int32 AGsEditorBaseTerrain::GetUpperIndex(int32 in_start, int32 in_end)
{
	if (in_start < in_end)
	{
		return in_end;
	}
	else
	{
		return in_start;
	}
}

void AGsEditorBaseTerrain::RemovePillar(int32 in_index)
{
	if (_PillarArray.IsValidIndex(in_index))
	{
		int32 last = _PillarArray.Num() - 1;

		UGsEditorTerrainPillarComp* pillar = _PillarArray[in_index];

		if (pillar)
		{
			_PillarArray.RemoveAt(in_index);
			
			//Destory pillar using sscs editor	

			if (TryRemovePlane())
			{
				Draw();
			}
		}
	}
}

bool AGsEditorBaseTerrain::TryRemovePlane()
{
	UGsEditorTerrainPlaneComp* comp = _PlaneArray.Last();
	_PlaneArray.Remove(comp);

	if (comp)
	{		
		//Destory plane using sscs editor		

		return true;
	}

	return false;
}