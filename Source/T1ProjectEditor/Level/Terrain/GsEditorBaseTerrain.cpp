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

	_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	_Spline->SetRelativeLocation(FVector::ZeroVector);

	RootComponent = _Root;

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
		iter->DestroyComponent();
	}

	_PillarArray.Empty();

	const TArray<UActorComponent*>& planeArray = GetComponentsByClass(UGsEditorTerrainPlaneComp::StaticClass());

	for (UActorComponent* iter : planeArray)
	{
		iter->DestroyComponent();
	}

	_PlaneArray.Empty();
}

void AGsEditorBaseTerrain::ConstructFence()
{
	for (int32 i = 0; i < _PointArray.Num(); ++i)
	{
		UGsEditorTerrainPillarComp* pillar = NewObject<UGsEditorTerrainPillarComp>(this);
		pillar->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		/*pillar->RegisterComponent();*/

		//UGsEditorTerrainPillarComp* pillar = ConstructObject<UGsEditorTerrainPillarComp>(UGsEditorTerrainPillarComp::StaticClass(), this);

		RegisterPillar(pillar, i);

		pillar->SetWorldLocation(_PointArray[i]);
	}

	//Create pillar 
	for (FVector& iter : _PointArray)
	{
		UGsEditorTerrainPlaneComp* plane = NewObject<UGsEditorTerrainPlaneComp>(this);
		plane->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		/*plane->RegisterComponent();*/

		//UGsEditorTerrainPlaneComp* plane = ConstructObject<UGsEditorTerrainPlaneComp>(UGsEditorTerrainPillarComp::StaticClass(), this);

		RegisterPlane(plane);
	}

	RegisterAllComponents();
}

// Called when the game starts or when spawned
void AGsEditorBaseTerrain::BeginPlay()
{
	Super::BeginPlay();

	Destroy();
}

void AGsEditorBaseTerrain::Draw()
{
	for(int i = 0; i < _PointArray.Num(); ++i)	
	{
		UGsEditorTerrainPillarComp* pillar = _PillarArray[i];		

		if (pillar)
		{
			pillar->SetWorldLocation(_PointArray[i]);
			pillar->Draw(_PillarColor);
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
						_PlaneArray[last]->Draw(_PillarArray[last], _PillarArray[0], _PlaneColor);
					}
				}
			}
			else
			{
				if (_PlaneArray[i])
				{
					if (_PillarArray[i] && _PillarArray[i + 1])
					{
						_PlaneArray[i]->Draw(_PillarArray[i], _PillarArray[i + 1], _PlaneColor);
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
		in_pillar->_Index = in_index;
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
		//FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked< FLevelEditorModule >("LevelEditorModuel");

		//TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();

		//TSharedPtr<SDockTab> tab = LevelEditorTabManager->FindExistingLiveTab(FName("LevelEditorSelectionDetails"));

		//if (tab.IsValid())
		//{
		//	TSharedPtr<SWidget> tabContent = tab->GetContent();

		//	if (tabContent.IsValid())
		//	{
		//		//FString test = "name : " + tabContent->GetTypeAsString();

		//		UE_LOG(LogTemp, Log, TEXT("name : %s"), *tabContent->GetTypeAsString());
		//	}

		//	//TSharedRef<SActorDetails> actorDetils = StaticCastSharedPtr<SActorDetails>(tabContent);

		//	//if(actorDetils.IsValid())
		//}

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