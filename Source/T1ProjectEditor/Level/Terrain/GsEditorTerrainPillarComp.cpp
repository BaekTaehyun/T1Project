// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainPillarComp.h"
#include "./GsEditorBaseTerrain.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

// Sets default values for this component's properties
UGsEditorTerrainPillarComp::UGsEditorTerrainPillarComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bWantsOnUpdateTransform = true;
}

void UGsEditorTerrainPillarComp::Draw()
{
	FVector v1, v2, v3, v4, v5, v6, v7, v8;
	float factor = 10.0f;
	FVector height = FVector(0, 0, 100);

	v1 = FVector(-1, -1, 0) * factor;
	v2 = FVector(1, -1, 0) * factor;
	v3 = FVector(1, 1, 0) * factor;
	v4 = FVector(-1, 1, 0) * factor;

	v5 = FVector(-1, -1, 0) * factor + height;
	v6 = FVector(1, -1, 0) * factor + height;
	v7 = FVector(1, 1, 0) * factor + height;
	v8 = FVector(-1, 1, 0) * factor + height;

	//vertex
	TArray<FVector> vertexs;

	vertexs.Add(v1);
	vertexs.Add(v2);
	vertexs.Add(v3);
	vertexs.Add(v4);

	vertexs.Add(v5);
	vertexs.Add(v6);
	vertexs.Add(v7);
	vertexs.Add(v8);

	//triangle
	TArray<int32> triangles;

	//bottom
	AddTriangle(0, 1, 2, triangles);
	AddTriangle(0, 2, 3, triangles);

	//Up
	AddTriangle(4, 6, 5, triangles);
	AddTriangle(4, 7, 6, triangles);

	//front
	AddTriangle(0, 5, 1, triangles);
	AddTriangle(0, 4, 5, triangles);

	//back
	AddTriangle(3, 2, 6, triangles);
	AddTriangle(3, 6, 7, triangles);

	//Right
	AddTriangle(4, 6, 5, triangles);
	AddTriangle(4, 7, 6, triangles);

	//right
	AddTriangle(1, 6, 2, triangles);
	AddTriangle(1, 5, 6, triangles);

	//Left
	AddTriangle(0, 3, 7, triangles);
	AddTriangle(0, 7, 4, triangles);

	//normal
	TArray<FVector> normals;

	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	//uv0
	TArray<FVector2D> uv;

	uv.Add(FVector2D(0, 0));
	uv.Add(FVector2D(0, 1));
	uv.Add(FVector2D(1, 1));
	uv.Add(FVector2D(1, 0));

	//tangents
	TArray<FProcMeshTangent> tangents;

	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	//vertex color
	TArray<FLinearColor> vertexColors;

	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);

	CreateMeshSection_LinearColor(0, vertexs, triangles, normals, uv, vertexColors, tangents, true);
	ContainsPhysicsTriMeshData(true);

	if (_Parent)
	{
		if (_Parent->_PlaneMaterial)
		{
			UMaterialInstanceDynamic* instanceMaterial = UMaterialInstanceDynamic::Create(_Parent->_PlaneMaterial, this);
			instanceMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(100.0f, 100.0f, 0.0f));
			SetMaterial(0, instanceMaterial);
		}
	}
}

void UGsEditorTerrainPillarComp::OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport)
{
	if (_Parent)
	{
		UE_LOG(LogTemp, Log, TEXT("Component location : %s"), *GetComponentLocation().ToString());

		_Parent->_PointArray[_Index] = GetComponentLocation();
		_Parent->Draw();
	}
}

void UGsEditorTerrainPillarComp::AddTriangle(int32 v1, int32 v2, int32 v3, TArray<int32>& in_triangleArray)
{
	in_triangleArray.Add(v1);
	in_triangleArray.Add(v2);
	in_triangleArray.Add(v3);
}

