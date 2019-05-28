// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsEditorBaseTerrain.generated.h"

class UGsEditorPillarComp;
class UGsEditorTerrainPlaneComp;

UCLASS()
class T1PROJECTEDITOR_API AGsEditorBaseTerrain : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorPillarComp*> _PillarArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorTerrainPlaneComp*> _PlaneArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PillarMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PlaneMaterial;

public:
	// Sets default values for this actor's properties
	AGsEditorBaseTerrain();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& in_transform) override;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void AddPillar(UGsEditorPillarComp* in_pillar);
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void AddPlane(UGsEditorTerrainPlaneComp* in_plane);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Draw();

};
