// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GsEditorTerrainPillarComp.generated.h"

class AGsEditorBaseTerrain;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class T1PROJECTEDITOR_API UGsEditorTerrainPillarComp : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		AGsEditorBaseTerrain* _Parent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		int32 _Index;

public:	
	// Sets default values for this component's properties
	UGsEditorTerrainPillarComp();
	void Draw(FColor in_color);
	// Called every frame
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport) override;
	void AddTriangle(int32 v1, int32 v2, int32 v3, TArray<int32>& in_triangleArray);

		
};
