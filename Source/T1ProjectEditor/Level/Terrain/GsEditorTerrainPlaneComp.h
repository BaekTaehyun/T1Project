// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GsEditorTerrainPlaneComp.generated.h"

class AGsEditorBaseTerrain;
class UGsEditorPillarComp;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class T1PROJECTEDITOR_API UGsEditorTerrainPlaneComp : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		AGsEditorBaseTerrain* _Parent;

public:
	// Sets default values for this component's properties
	UGsEditorTerrainPlaneComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Draw(UGsEditorPillarComp* in_start, UGsEditorPillarComp* in_end);
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport) override;

};
