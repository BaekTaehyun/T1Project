// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GsEditorTerrainPillarComp.generated.h"

class AGsEditorTerrainShape;

//----------------------------------------------------------------
// AGsEditorBaseTerrain의 포인트 좌표를 위한 펜스
//----------------------------------------------------------------

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class T1PROJECTEDITOR_API UGsEditorTerrainPillarComp : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		AGsEditorTerrainShape* _Parent;

public:	
	// Sets default values for this component's properties
	UGsEditorTerrainPillarComp();
	void Draw(FColor in_color, float in_height);
	// Called every frame
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport) override;
	void AddTriangle(int32 v1, int32 v2, int32 v3, TArray<int32>& in_triangleArray);

		
};
