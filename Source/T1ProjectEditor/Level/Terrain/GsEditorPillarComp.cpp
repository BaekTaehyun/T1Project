// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorPillarComp.h"
#include "./GsEditorBaseTerrain.h"

// Sets default values for this component's properties
UGsEditorPillarComp::UGsEditorPillarComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGsEditorPillarComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGsEditorPillarComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGsEditorPillarComp::Draw()
{
	//FVector v1, v2, v3, v4, v5, v6, v7, v8;
	//
	//v1 = FVector(-5, -5, 0);
	//v2 = FVector(-5, 5, 0);
	//v3 = FVector(5, 5, 0);
	//v4 = FVector(5, -5, 0);

	//v5 = FVector(-5, -5, 100);
	//v6 = FVector(-5, 5, 100);
	//v7 = FVector(5, 5, 100);
	//v8 = FVector(5, -5, 100);

	////vertex
	//TArray<FVector> vertexs;

	//vertexs.Add(v1);
	//vertexs.Add(v2);
	//vertexs.Add(v3);
	//vertexs.Add(v4);

	//vertexs.Add(v5);
	//vertexs.Add(v6);
	//vertexs.Add(v7);
	//vertexs.Add(v8);

	////triangle
	//TArray<int32> triangles;

	////bottom
	//triangles.Add(0);
	//triangles.Add(1);
	//triangles.Add(2);

	//triangles.Add(2);



}

void UGsEditorPillarComp::OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport)
{
	if (_Parent)
	{
		_Parent->Draw();
	}
}