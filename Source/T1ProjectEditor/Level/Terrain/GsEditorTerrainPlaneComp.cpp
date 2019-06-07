// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainPlaneComp.h"
#include "./GsEditorTerrainPillarComp.h"
#include "./GsEditorBaseTerrain.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

// Sets default values for this component's properties
UGsEditorTerrainPlaneComp::UGsEditorTerrainPlaneComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UGsEditorTerrainPlaneComp::Draw(UGsEditorTerrainPillarComp* in_start, UGsEditorTerrainPillarComp* in_end, float in_height, FColor in_outerColor, FColor in_insideColor)
{
	if (in_start && in_end)
	{			
		FVector start, end;
		FVector v1, v2, v3, v4;

		start = in_start->GetComponentLocation();
		end = in_end->GetComponentLocation();

		FVector location = (start + end) / 2;
		SetWorldLocation(location);

		v1 = start - location;
		v2 = end - location;
		v3 = v2 + FVector::FVector(0, 0, in_height);
		v4 = v1 + FVector::FVector(0, 0, in_height);

		if (_Parent)
		{
			DrawPlane(0, v1, v2, v3, v4, true, _Parent->_PlaneOuterColor);
			DrawPlane(1, v1, v2, v3, v4, false, _Parent->_PlaneInsideColor);
		}
			
		SetNoCollision();		
	}	
}

void UGsEditorTerrainPlaneComp::DrawPlane(int32 in_section, FVector in_v1, FVector in_v2, FVector in_v3, FVector in_v4, bool in_clockOrient, FColor in_color)
{
	//vertex;
	TArray<FVector> vertexs;

	vertexs.Add(in_v1);
	vertexs.Add(in_v2);
	vertexs.Add(in_v3);
	vertexs.Add(in_v4);

	//triangle
	TArray<int32> triangles;

	if (in_clockOrient)
	{
		triangles.Add(0);
		triangles.Add(2);
		triangles.Add(1);

		triangles.Add(0);
		triangles.Add(3);
		triangles.Add(2);		
	}
	else
	{
		triangles.Add(0);
		triangles.Add(1);
		triangles.Add(2);

		triangles.Add(0);
		triangles.Add(2);
		triangles.Add(3);
	}
	
	//normal
	TArray<FVector> normals;

	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));
	normals.Add(FVector(0, 0, 1));

	//uv0
	TArray<FVector2D> uv;

	uv.Add(FVector2D(0, 0));
	uv.Add(FVector2D(0, 1));
	uv.Add(FVector2D(1, 1));
	uv.Add(FVector2D(1, 0));

	//tangents
	TArray<FProcMeshTangent> tangents;

	tangents.Add(FProcMeshTangent(0, 0, 1));
	tangents.Add(FProcMeshTangent(0, 0, 1));
	tangents.Add(FProcMeshTangent(0, 0, 1));

	//vertex color
	TArray<FLinearColor> vertexColors;

	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);
	vertexColors.Add(FLinearColor::Blue);

	CreateMeshSection_LinearColor(in_section, vertexs, triangles, normals, uv, vertexColors, tangents, false);

	SetPlaneMaterial(in_color, in_section);
}

void UGsEditorTerrainPlaneComp::SetNoCollision()
{
	ContainsPhysicsTriMeshData(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel14);
	SetNotifyRigidBodyCollision(false);
	SetGenerateOverlapEvents(false);
	SetCastShadow(true);
}

void UGsEditorTerrainPlaneComp::SetPlaneMaterial(FColor in_color, int32 in_section)
{
	if (_Parent)
	{
		if (_Parent->_Material)
		{
			UMaterialInstanceDynamic* instanceMaterial = UMaterialInstanceDynamic::Create(_Parent->_Material, this);
			instanceMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(in_color));
			Super::SetMaterial(in_section, instanceMaterial);
		}
	}
}