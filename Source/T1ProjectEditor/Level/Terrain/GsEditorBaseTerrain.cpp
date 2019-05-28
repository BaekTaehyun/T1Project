// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorBaseTerrain.h"
#include "./GsEditorPillarComp.h"
#include "./GsEditorTerrainPlaneComp.h"

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

	const TArray<UActorComponent*>& pillarArray = GetComponentsByClass(UGsEditorPillarComp::StaticClass());

	for (UActorComponent* iter : pillarArray)
	{
		if (iter)
		{
			_PillarArray.Add(Cast<UGsEditorPillarComp>(iter));
		}		
	}		

	_PlaneArray.Empty();

	const TArray<UActorComponent*>& planeArray = GetComponentsByClass(UGsEditorTerrainPlaneComp::StaticClass());

	for (UActorComponent* iter : planeArray)
	{
		if (iter)
		{
			_PlaneArray.Add(Cast<UGsEditorTerrainPlaneComp>(iter));
		}		
	}		

	Draw();
}

// Called when the game starts or when spawned
void AGsEditorBaseTerrain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGsEditorBaseTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGsEditorBaseTerrain::Draw()
{
	for (UGsEditorPillarComp* iter : _PillarArray)
	{
		if (iter)
		{
			iter->Draw();
		}
	}

	int num = _PillarArray.Num();
	int last = num - 1;

	if (_PlaneArray.Num() > 0)
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

void AGsEditorBaseTerrain::AddPillar(UGsEditorPillarComp* in_pillar)
{
	if (in_pillar)
	{
		_PillarArray.Add(in_pillar);
		in_pillar->_Parent = this;
	}	
}

void AGsEditorBaseTerrain::AddPlane(UGsEditorTerrainPlaneComp* in_plane)
{
	if (in_plane)
	{
		_PlaneArray.Add(in_plane);
		in_plane->_Parent = this;
	}
}