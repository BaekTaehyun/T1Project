// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GsEditorTerrainPlaneComp.generated.h"

class AGsEditorBaseTerrain;
class UGsEditorTerrainPillarComp;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class T1PROJECTEDITOR_API UGsEditorTerrainPlaneComp : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		AGsEditorBaseTerrain* _Parent;

private:
	UPROPERTY()
		UGsEditorTerrainPillarComp* _Start;
	UPROPERTY()
		UGsEditorTerrainPillarComp* _End;

public:
	// Sets default values for this component's properties
	UGsEditorTerrainPlaneComp();
	void Draw(UGsEditorTerrainPillarComp* in_start, UGsEditorTerrainPillarComp* in_end, FColor in_color);
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport) override;

};
