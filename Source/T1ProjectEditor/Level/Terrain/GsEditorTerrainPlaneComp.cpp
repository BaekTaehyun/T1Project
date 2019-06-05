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
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bWantsOnUpdateTransform = true;
}

void UGsEditorTerrainPlaneComp::Draw(UGsEditorTerrainPillarComp* in_start, UGsEditorTerrainPillarComp* in_end, FColor in_color)
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
		v3 = v2 + FVector::FVector(0, 0, 100);
		v4 = v1 + FVector::FVector(0, 0, 100);		

		//vertex;
		TArray<FVector> vertexs;

		vertexs.Add(v1);
		vertexs.Add(v2);
		vertexs.Add(v3);
		vertexs.Add(v4);

		//triangle
		TArray<int32> triangles;

		triangles.Add(0);
		triangles.Add(2);
		triangles.Add(1);

		triangles.Add(0);
		triangles.Add(3);
		triangles.Add(2);

		//normal
		TArray<FVector> normals;

		/*normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));*/

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

		CreateMeshSection_LinearColor(0, vertexs, triangles, normals, uv, vertexColors, tangents, false);

		//Set Ignore collision
		ContainsPhysicsTriMeshData(false);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);		
		SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel14);
		SetNotifyRigidBodyCollision(false);
		SetGenerateOverlapEvents(false);

		bCastShadowAsTwoSided = true;
		
		if (_Parent)
		{
			if (_Parent->_PlaneMaterial)
			{
				UMaterialInstanceDynamic* instanceMaterial = UMaterialInstanceDynamic::Create(_Parent->_PlaneMaterial, this);
				instanceMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(in_color));
				SetMaterial(0, instanceMaterial);
			}			
		}	
	}	
}

void UGsEditorTerrainPlaneComp::OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport)
{
	if (_Parent)
	{
		_Parent->Draw();
	}
}