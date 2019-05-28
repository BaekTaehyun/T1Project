// Fill out your copyright notice in the Description page of Project Settings.


#include "GsTerrainActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "T1ProjectEditor/Level/Terrain/GsTerrainPoint.h"

AGsTerrainActor::AGsTerrainActor()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGsTerrainActor::OnConstruction(const FTransform& in_transfrom)
{
	Super::OnConstruction(in_transfrom);

	//Draw();

	DrawAll();
}

void AGsTerrainActor::Tick(float in_deltaTime)
{
	Super::Tick(in_deltaTime);	

	//FlushPersistentDebugLines(GetWorld());

	/*DrawLines();
	
	DrawRects();*/
}

void AGsTerrainActor::TickActor(float in_deltaTime, enum ELevelTick in_tickType, FActorTickFunction& in_thisTickFunction)
{
	Super::TickActor(in_deltaTime, in_tickType, in_thisTickFunction);

	/*if (in_tickType == ELevelTick::LEVELTICK_TimeOnly)
	{
		UE_LOG(LogTemp, Log, TEXT("LEVELTICK_TimeOnly : %s"), *GetName());

		DrawLines();

		DrawRects();
	}
	else if (in_tickType == ELevelTick::LEVELTICK_ViewportsOnly)
	{
		UE_LOG(LogTemp, Log, TEXT("LEVELTICK_ViewportsOnly : %s"), *GetName());

		DrawLines();

		DrawRects();
	}*/
}

bool AGsTerrainActor::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AGsTerrainActor::Draw()
{
	DrawLines();
	DrawRects();
}

void AGsTerrainActor::DrawAll()
{
	TArray<AActor*> terrainArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld()
		, AGsTerrainActor::StaticClass()
		, terrainArray);

	AGsTerrainActor* terrain;

	FlushPersistentDebugLines(GetWorld());

	for (AActor* iter : terrainArray)
	{
		if (iter)
		{
			terrain = Cast<AGsTerrainActor>(iter);

			if (terrain)
			{
				terrain->DrawLines();
				terrain->DrawRects();
			}
		}
	}
}

void AGsTerrainActor::DrawLines()
{
	const TArray<UActorComponent*>& pointArray = GetComponentsByClass(UGsTerrainPoint::StaticClass());
	int num = pointArray.Num();	

	if (num >= 3)
	{				
		int lineNum = num - 1;

		for (int i = 0; i < lineNum; ++i)
		{
			DrawLine(pointArray[i], pointArray[i + 1]);
		}

		UActorComponent* last = pointArray.Last();

		if (last)
		{
			DrawLine(last, pointArray[0]);
		}
	}
}

void AGsTerrainActor::DrawLine(UActorComponent* in_startComp, UActorComponent* in_endComp)
{
	FVector startLocation, endLocation;
	USceneComponent* startPoint = nullptr;
	USceneComponent* endPoint = nullptr;
	
	if (nullptr != in_startComp
		&& nullptr != in_endComp)
	{
		startPoint = Cast<USceneComponent>(in_startComp);
		endPoint = Cast<USceneComponent>(in_endComp);

		if (nullptr != startPoint
			&& nullptr != endPoint)
		{
			startLocation = startPoint->GetComponentLocation();
			endLocation = endPoint->GetComponentLocation();

			DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Yellow, true, -1, 0, 1);
		}
	}
}

void AGsTerrainActor::DrawRects()
{
	const TArray<UActorComponent*>& pointArray = GetComponentsByClass(UGsTerrainPoint::StaticClass());
	int num = pointArray.Num();

	if (num >= 3)
	{
		int lineNum = num - 1;

		for (int i = 0; i < lineNum; ++i)
		{
			DrawRect(pointArray[i], pointArray[i + 1]);
		}

		UActorComponent* last = pointArray.Last();

		if (last)
		{
			DrawRect(last, pointArray[0]);
		}
	}
}

void AGsTerrainActor::DrawRect(UActorComponent* in_startComp, UActorComponent* in_endComp)
{
	FVector startLocation, endLocation;
	USceneComponent* startPoint = nullptr;
	USceneComponent* endPoint = nullptr;

	if (nullptr != in_startComp
		&& nullptr != in_endComp)
	{
		startPoint = Cast<USceneComponent>(in_startComp);
		endPoint = Cast<USceneComponent>(in_endComp);		

		if (nullptr != startPoint
			&& nullptr != endPoint)
		{
			startLocation = startPoint->GetComponentLocation();
			endLocation = endPoint->GetComponentLocation();

			FVector v1, v2, v3, v4;

			v1 = startLocation;
			v2 = endLocation;
			v3 = endLocation + FVector::FVector(0, 0, 100);
			v4 = startLocation + FVector::FVector(0, 0, 100);

			TArray<FVector> vertexArray;

			vertexArray.Add(v1);
			vertexArray.Add(v2);
			vertexArray.Add(v3);
			vertexArray.Add(v4);			

			TArray<int32> indexArray;

			indexArray.Add(0);
			indexArray.Add(1);
			indexArray.Add(2);

			indexArray.Add(0);
			indexArray.Add(2);
			indexArray.Add(3);

			DrawDebugMesh(GetWorld(), vertexArray, indexArray, _DebugMeshColor, true, -1, 1);
		}
	}
}